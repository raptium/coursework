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
    private String media_dir = "c:\\temp\\media\\";
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
    
    @Override
    public void run(){
        HttpRequest request = m_server.getRequest(m_channel);
        String path = null;
        FileInputStream iStream = null;
        streamClient sClient = null;
        
        try {
            path = URLDecoder.decode(request.getPath(), "utf-8"); //decode the URL escape character
        } catch(UnsupportedEncodingException ex) {
            path = request.getPath();
        }
        try {
            parsePath(path);
        } catch(Exception ex){
            System.out.println("Invalid request.");
            return;
        }
        
        
        try{
            iStream = new FileInputStream(new File(media_dir + m_filename));
        }catch(Exception ex){
            System.out.println("Canot find the requested file in cache.");
            System.out.println("Try to get the file from streamming server.");
            
            try {
                sClient = new streamClient(m_host, m_port, m_packetSize, m_protocol, m_filename);
                sClient.connect();
            } catch (Exception ex1){
                System.out.println("Server connect failed.");
                Logger.getLogger(Main.class.getName()).log(Level.SEVERE, null, ex1);
                
                try{
                    HttpResponse response = new HttpResponse();
                    response.setStatus("404"); //HTTP 404
                    response.fillMessage("Cannot find the request file.".getBytes());
                    response.setHeader("Connection", "close");
                    m_server.sendResponse(m_channel, response);
                    m_channel.close();
                }catch(Exception ex2){
                    
                    
                }
                    
                return;
            }
        }
        
        System.out.print(request.toString());
        byte[] buffer = new byte[1024];

        try{
            
            HttpResponse response = new HttpResponse();
            response.setStatus("200"); //HTTP 200 OK
            //Set the default MIME type
            response.setHeader("Content-Type", "application/octet-stream"); 
            
            
            Pattern pattern = Pattern.compile("\\.(.*)$"); 
            Matcher matcher = pattern.matcher(m_filename);
            boolean found = matcher.find(); //Get the file extension
            if(found){
                String ext = matcher.group(1);
                if(m_mime.get(ext) != null) //Lookup the hashtable
                    response.setHeader("Content-Type", m_mime.get(ext)); //Set the correct MIME type
            }

            response.setHeader("Connection", "close");
            response.fillMessage(new String("").getBytes()); //Add empty data message
            //Set the Content-Length to the whole length of the file
            //response.setHeader("Content-Length", "" + iStream.available());
            m_server.sendResponse(m_channel, response); //Send the HTTP response with header only
            
            //For transmission rate control
            long startTime = System.currentTimeMillis();
            int sent = 0;
            long sleepTime;
            double speed = 1024 * 256; //256KB/s should be enough for streaming
            int packetSize = buffer.length;

            while(true) {
                int length;
                if(iStream != null)
                    length = iStream.read(buffer);
                else{
                    length = sClient.read(buffer);
                    if(sent == 0){
                        sent++;
                        continue;
                    }
                }
                if(length <= 0)
                    break;
                m_server.sendData(m_channel, buffer, length); //Send the streaming data
                
                //rate control
                sent ++;
                sleepTime = (long) (packetSize / speed * 1000 * sent - 
                        System.currentTimeMillis() + startTime);
                if(sleepTime > 0)
                    sleep(sleepTime);
            }
            if(iStream != null)
                iStream.close();
            m_channel.close();
        }catch(Exception ex){
            Logger.getLogger(Main.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
}
