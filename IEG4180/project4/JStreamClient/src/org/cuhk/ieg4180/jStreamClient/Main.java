package org.cuhk.ieg4180.jStreamClient;


import java.util.logging.Level;
import java.util.logging.Logger;
import org.cuhk.ieg4180.jStreamClient.libHTTP.*;
import org.cuhk.ieg4180.jStreamClient.libStream.*;
import java.io.*;
import java.net.URLDecoder;
import java.nio.channels.SocketChannel;
import java.util.*;
import java.util.regex.*;

/**
 *
 * @author hguan5
 */
public class Main {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        try {
            HttpServer server = new HttpServer("localhost", 80);
            
            server.start();
            while(true){
                try {
                    SocketChannel channel = server.accept();
                    mediaServer mserver = new mediaServer(server, channel);
                    mserver.start();
                } catch (Exception ex){
                    Logger.getLogger(Main.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
        } catch (Exception ex) {
            Logger.getLogger(Main.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
}

class mediaServer extends Thread{
    
    private SocketChannel m_channel;
    private HttpServer m_server;
    private String m_cacheDir = "c:\\temp\\media\\cache\\";
    private Hashtable<String, String> m_mime;
    private String m_filename;
    private int m_port;
    private String m_protocol;
    private String m_host;
    private int m_packetSize;
    
   
    
    
    mediaServer(HttpServer server, SocketChannel channel){
        m_channel = channel;
        m_server = server;
        m_mime = new Hashtable<String, String>();
        m_mime.put("mpeg", "video/mpeg");
        m_mime.put("mp3", "audio/mpeg");
        m_mime.put("rmvb", "application/vnd.rn-realmedia-vbr");
        m_mime.put("wmv", "video/x-ms-wmv");
    }
    
    
    private void parsePath(String path) throws Exception{
        
        Pattern pattern = Pattern.compile("^/([a-zA-Z0-9\\.]+):(\\d+)/(\\d+)/(tcp|udp)/(.+)$");
        Matcher matcher = pattern.matcher(path);
        boolean found = matcher.find();
        if(found){
            m_host = matcher.group(1);
            m_port = Integer.valueOf(matcher.group(2));
            m_packetSize = Integer.valueOf(matcher.group(3));
            m_protocol = matcher.group(4);
            m_filename = matcher.group(5);
        }
        else
            throw new Exception();
    }
    
    
    private void Error404(Exception ex){
        HttpResponse response = new HttpResponse();
        
        try{
            response.setStatus("404");
            response.setHeader("Connection", "close");
            response.setHeader("Content-Type", "text/html;charset=iso-8859-1");
            response.fillMessage(ex.getMessage().getBytes());
            m_server.sendResponse(m_channel, response);
            m_channel.close();
        }catch(Exception e){
            
        }
    }
    
    private void send200(){
        HttpResponse response = new HttpResponse();
        
        try{
            response.setStatus("200");
            response.setHeader("Connection", "close");
            response.setHeader("Content-Type", "text/html;charset=iso-8859-1");
            response.fillMessage("".getBytes());
            m_server.sendResponse(m_channel, response);
            m_channel.close();
        }catch(Exception e){
            
        }
    }
    
    
    @Override
    public void run(){
        HttpRequest request = m_server.getRequest(m_channel);
        String path = null;
        FileInputStream iStream = null;
        streamClient sClient = null;
        
        
        // decode the URL escape character if possible
        try {
            path = URLDecoder.decode(request.getPath(), "utf-8"); 
        } catch(UnsupportedEncodingException ex) {
            path = request.getPath();
        }
        
        
        //  parse the parameters from the PATH field of a HTTP Request
        try {
            parsePath(path);
        } catch(Exception ex){
            System.out.println("Invalid request."); // Cannot continue if the request is invalid
            Error404(ex);
            return; // Error 404 here
        }
        
        
        try{
            // Try to find the file in cache directory first
            iStream = new FileInputStream(new File(m_cacheDir + m_filename));
        }catch(Exception ex){
            System.out.println("File not found in cache.");
            // It seems we cannot find the file in the cache directory
            // Therefore send the request to the stream server
            
            try {
                // Try to connect to the stream server
                sClient = new streamClient(m_host, m_port, m_packetSize, m_protocol, m_filename);
                sClient.connect();
            } catch (Exception ex1){
                // Cannot connect to the stream server
                
                System.out.println("Server connect failed.");
                Error404(ex1);
                return; // Error 404 here
            }
        }
        
        
        /*
        if(!request.toString().contains("Pragma")){
            send200();
            return;
        }
        */
        
        System.out.print(request.toString()); // for debug only

        
        int len = 0;
        try{
            if(iStream != null){
                len = iStream.available();
            }else{
                len = sClient.getLength();
            }
        }catch(Exception ex){
            
        }

        byte[] buffer = new byte[m_packetSize];
        try{
            System.out.println("here.");
            
            HttpResponse response = new HttpResponse();
            response.setStatus("200"); // HTTP 200 OK
            // Set the default MIME type
            response.setHeader("Content-Type", "application/octet-stream"); 
            
            
            Pattern pattern = Pattern.compile("\\.(.*)$"); 
            Matcher matcher = pattern.matcher(m_filename);
            boolean found = matcher.find(); // Get the file extension
            if(found){
                String ext = matcher.group(1);
                if(m_mime.get(ext) != null) // Lookup the hashtable
                    response.setHeader("Content-Type", m_mime.get(ext)); // Set the correct MIME type
            }

            response.setHeader("Connection", "close");
            response.fillMessage(new String("").getBytes()); // Add empty data message
            // Set the Content-Length to the whole length of the file
            response.setHeader("Content-Length", "" + len);
            m_server.sendResponse(m_channel, response); // Send the HTTP response with header only
            
            // For transmission rate control
            long startTime = System.currentTimeMillis();
            int sent = 0;
            long sleepTime;
            double speed = 1024 * 256; // 256KB/s should be enough for streaming
            int packetSize = buffer.length;
            int received = 0;
            
            while(true) {
                int length;
                if(iStream != null) // If we are reading from the cache file
                    length = iStream.read(buffer);
                else{
                    // If we are streaming form the server
                    length = sClient.read(buffer);
                }
                if(length <= 0)
                    break;
                received += length;
                m_server.sendData(m_channel, buffer, length); //Send the streaming data
                if(received >= len)
                    break;
                
                //rate control
                sent ++;
                sleepTime = (long) (packetSize / speed * 1000 * sent - 
                        System.currentTimeMillis() + startTime);
                if(sleepTime > 0)
                    sleep(sleepTime);
            }
            if(iStream != null)
                iStream.close();
            else
                sClient.finish();
            m_channel.close();
        }catch(Exception ex){
            Logger.getLogger(Main.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
}
