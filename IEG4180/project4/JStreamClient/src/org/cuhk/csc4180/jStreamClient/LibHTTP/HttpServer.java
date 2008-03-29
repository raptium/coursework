/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cuhk.csc4180.jStreamClient.LibHTTP;

import java.io.EOFException;
import java.io.IOException;
import java.net.*;
import java.nio.*;
import java.nio.channels.*;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author hguan5
 */
public class HttpServer extends Thread{
    private String m_hostname;
    private int m_port;
    private InetSocketAddress m_addr;
    private ServerSocketChannel m_acceptChannel;
    private SocketChannel m_activeChannel;
    
    public HttpServer(){
        m_hostname = "localhost";
        m_port = 80;
    }
    
    public HttpServer(String hostname, int port){
        m_hostname = hostname;
        m_port = port;
    }
    
    @Override
    public void run(){
        try {
            m_addr = new InetSocketAddress(m_hostname, m_port);
            m_acceptChannel = ServerSocketChannel.open();
            m_acceptChannel.configureBlocking(true);
            m_acceptChannel.socket().bind(m_addr);
            
        } catch (IOException ex) {
            Logger.getLogger(HttpServer.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    
    public HttpRequest getRequest(){
        HttpRequest m_request = new HttpRequest();
        ByteBuffer buffer = ByteBuffer.allocate(4096);
        try {
            m_activeChannel = m_acceptChannel.accept();
            m_activeChannel.read(buffer);
            m_request.parse(buffer.array());
        } catch (IOException ex) {
            Logger.getLogger(HttpServer.class.getName()).log(Level.SEVERE, null, ex);
        } catch (Exception ex) {
            Logger.getLogger(HttpServer.class.getName()).log(Level.SEVERE, null, ex);
        }
        return m_request;
    }
    
    public void sendResponse(HttpResponse response){
        ByteBuffer buffer = ByteBuffer.allocate(response.toBytes().length);
        buffer.put(response.toBytes());
        int attempts = 0;
        buffer.rewind();
        try {
            while(buffer.hasRemaining()){
                int len = m_activeChannel.write(buffer);
                attempts++;
                if (len < 0){
                    throw new EOFException();
                }
                System.out.println("bytes sent: " + len);
            }
            m_activeChannel.close();
        } catch (IOException ex) {
            Logger.getLogger(HttpServer.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
}
