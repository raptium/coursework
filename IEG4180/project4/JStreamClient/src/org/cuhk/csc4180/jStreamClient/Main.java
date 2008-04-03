package org.cuhk.csc4180.jStreamClient;


import java.util.logging.Level;
import java.util.logging.Logger;
import org.cuhk.csc4180.jStreamClient.LibHTTP.*;
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
    
    mediaServer(HttpServer server, SocketChannel channel){
        m_channel = channel;
        m_server = server;
        m_mime = new Hashtable<String, String>();
        m_mime.put("mpeg", "video/mpeg");
        m_mime.put("mp3", "audio/mpeg");
        m_mime.put("rmvb", "application/vnd.rn-realmedia-vbr");
    }
    
    @Override
    public void run(){
        HttpRequest request = m_server.getRequest(m_channel);
        String path = null;
        try {
            path = URLDecoder.decode(request.getPath(), "utf-8"); //decode the URL escape character
        } catch(UnsupportedEncodingException ex) {
            path = request.getPath();
        }
        FileInputStream iStream = null;
        try{
            iStream = new FileInputStream(new File(media_dir + path.substring(1)));
        }catch(Exception ex){
            System.out.println("Canot find the requested file in cache.");
            Logger.getLogger(mediaServer.class.getName()).log(Level.SEVERE, null, ex);
            try {
                HttpResponse response = new HttpResponse();
                response.setStatus("404");
                response.fillMessage(new String("").getBytes());
                m_server.sendResponse(m_channel, response);
                m_channel.close();
                return;
            } catch (Exception ex1) {
                Logger.getLogger(mediaServer.class.getName()).log(Level.SEVERE, null, ex1);
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
            Matcher matcher = pattern.matcher(path);
            boolean found = matcher.find(); //Get the file extension
            if(found){
                String ext = matcher.group(1);
                if(m_mime.get(ext) != null) //Lookup the hashtable
                    response.setHeader("Content-Type", m_mime.get(ext)); //Set the correct MIME type
            }

            response.setHeader("Connection", "close");
            response.fillMessage(new String("").getBytes()); //Add empty data message
            //Set the Content-Length to the whole length of the file
            response.setHeader("Content-Length", "" + iStream.available());
            m_server.sendResponse(m_channel, response); //Send the HTTP response with header only
            
            //For transmission rate control
            long startTime = System.currentTimeMillis();
            int sent = 0;
            long sleepTime;
            double speed = 1024 * 256; //256KB/s should be enough for streaming
            int packetSize = buffer.length;

            while(true) {
                int length;
                length = iStream.read(buffer);
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
            iStream.close();
            m_channel.close();
        }catch(Exception ex){
            Logger.getLogger(Main.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
}
