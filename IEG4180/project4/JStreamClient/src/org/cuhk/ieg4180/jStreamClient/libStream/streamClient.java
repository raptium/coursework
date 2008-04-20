package org.cuhk.ieg4180.jStreamClient.libStream;

import java.io.*;
import java.net.*;
import java.nio.*;
import java.nio.channels.*;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * Streaming client with cache
 * @author hguan5
 */
public class streamClient {
    private SocketChannel m_tcpChannel;
    private DatagramChannel m_udpChannel;
    private InetSocketAddress m_remote;
    private String m_filename;
    private int m_packetSize;
    private int m_protocol;
    private mediaCache m_cache;
    private int m_contentLength;
    
    /**
     * 
     * @param host The hostname of the streaming server, either IP or domain name
     * @param port Port number of the streaming server
     * @param packetSize Packet Size for transmitting
     * @param protocol TCP or UDP protocol, 'tcp' or 'udp'
     * @param filename The filename requested
     * @throws java.io.IOException
     */
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
    
    /**
     * Swap Integer value between litten-endian and big-endian
     * 
     * @param v The int value to be converted
     * @return The converted int value
     */
    public final static int swabInt(int v) {
        return  (v >> 24) | (v << 24) |
                ((v << 8) & 0x00ff0000) | ((v >> 8) & 0x0000ff00);
    }
    
    /**
     * Connect to the streaming server, send file request, if it fails, IOException will be thrown
     * @throws java.io.IOException
     */
    public void connect() throws java.io.IOException{
        ByteBuffer buffer = ByteBuffer.allocate(m_packetSize);
        int ret;
        
        buffer.putInt(0, swabInt(m_packetSize));
        buffer.putInt(4, swabInt(256*1024)); // Set sending rate to 256kBps
        buffer.putInt(8, swabInt(-1));
        buffer.position(12);
        buffer.put(m_filename.getBytes());
        buffer.rewind();
        
        if(m_protocol == 1){
            m_tcpChannel.write(buffer);            
        }
        else{
            m_udpChannel.write(buffer);
        }
        
        ByteBuffer bBuffer = ByteBuffer.allocate(m_packetSize);
        try{
            if(m_protocol == 1){
                ret = m_tcpChannel.read(bBuffer);
            }
            else{
                ret = m_udpChannel.read(bBuffer);
            }
            m_contentLength = bBuffer.getInt(0);
        }catch(java.io.IOException ex){
            return;
        }
        
        // create cache after connected
        m_cache = new mediaCache(m_filename);
    }
    
    
    /**
     * Get the file length return by the streaming server.
     * @return File size in bytes
     */
    public int getLength(){
        return m_contentLength;
    }
    
    /**
     * Receive data from the server and fill the data into given buffer.
     * @param buffer Buffer to store the data from streaming server
     * @return receive data length in bytes
     */
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
        
        try{
            m_cache.write(buffer);
        } catch (IOException ex) {
            Logger.getLogger(streamClient.class.getName()).log(Level.SEVERE, null, ex);
        }
        
        return length;
    }
    
    /**
     * To finish the streaming, the temp file in cache will be saved.
     */
    public void finish(){
        try {
            m_cache.save();
        } catch (IOException ex) {
            Logger.getLogger(streamClient.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
}

/**
 * 
 * @author hguan5
 */
class mediaCache{
    private final String m_cacheDir = "c:\\temp\\media\\cache\\";
    private String m_filename;
    private FileOutputStream m_stream;
    private File m_tempFile;
    
    
    public mediaCache(String filename) throws FileNotFoundException, SecurityException{
        m_filename = filename;
        m_tempFile = new File(m_cacheDir + m_filename + ".tmp");
        m_stream = new FileOutputStream(m_tempFile);
    }
    
    public void write(byte[] buffer) throws IOException{
        m_stream.write(buffer);
    }
    
    public void save() throws IOException{
        m_stream.close();
        try{
            m_tempFile.renameTo(new File(m_cacheDir + m_filename));
        }
        catch (Exception ex) {
            Logger.getLogger(streamClient.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
}
