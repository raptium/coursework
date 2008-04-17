/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cuhk.ieg4180.jStreamClient.libStream;

import java.io.IOException;
import java.net.*;
import java.nio.*;
import java.nio.channels.*;

/**
 *
 * @author hguan5
 */
public class streamClient {
    private SocketChannel m_tcpChannel;
    private DatagramChannel m_udpChannel;
    private InetSocketAddress m_remote;
    private String m_filename;
    private int m_packetSize;
    
    private int m_protocol;
    
    
    public streamClient(String host, int port, int packetSize,String protocol, String filename) throws java.io.IOException{
        m_remote = new InetSocketAddress(host, port);
        
        if(protocol.equals("tcp"))
            m_protocol = 1;
        else
            m_protocol = 0;
        
        m_packetSize = packetSize;
        m_filename = filename;
        
        if(m_protocol == 1){
            m_tcpChannel = SocketChannel.open(m_remote);
        }
        else{
            m_udpChannel = DatagramChannel.open();
            m_udpChannel.connect(m_remote);
        }
    }
    
    public final static int swabInt(int v) {
        return  (v >> 24) | (v << 24) |
                ((v << 8) & 0x00FF0000) | ((v >> 8) & 0x0000FF00);
    }
    
    public void connect() throws java.io.IOException{
        ByteBuffer buffer = ByteBuffer.allocate(256);
        
        
        buffer.putInt(0, swabInt(m_packetSize));
        buffer.putInt(12, swabInt(10101));
        buffer.position(16);
        buffer.put(m_filename.getBytes());
        buffer.rewind();
        
        if(m_protocol == 1){
            m_tcpChannel.write(buffer);
            m_tcpChannel.read(buffer);
            buffer.putInt(0, swabInt(404));
            buffer.rewind();
            m_tcpChannel.write(buffer);
            
        }
        else{
            m_udpChannel.write(buffer);
        }
    }
    
    
    public int read(byte[] buffer){
        int length = 0;
        ByteBuffer bBuffer = ByteBuffer.allocate(buffer.length);
        
        try{
            if(m_protocol == 1){
                length = m_tcpChannel.read(bBuffer);
            }
            else{
                length = m_udpChannel.read(bBuffer);
            }
        }catch(java.io.IOException ex){
            return 0;
        }
        
        System.arraycopy(bBuffer.array(), 0, buffer, 0, buffer.length);
       
        return length;
    }
}
