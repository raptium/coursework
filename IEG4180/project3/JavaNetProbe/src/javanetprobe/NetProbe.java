/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package javanetprobe;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.*;
import java.nio.*;
import java.nio.channels.*;
import org.jvnet.substance.*;
import org.jvnet.substance.skin.*;
import org.jvnet.substance.title.*;




/**
 *
 * @author hguan5
 */
public class NetProbe implements Runnable{

    protected static NetProbeFrame frame;
    protected InetSocketAddress servAddr;
    protected static NetProbe probe;
    
    protected int PacketSize;
    protected int SendingRate;
    protected int NumPackets;
    
    protected double bytesTransferred;
    protected int packetsTransferred;
    protected int maxPacketNum;
    
    
    NetProbe(){
        bytesTransferred = 0;
        packetsTransferred = 0;
        maxPacketNum = -1;
    }
    
    /**
    * @param args the command line arguments
    */
    public static void main(String args[]) {
        try {
            UIManager.setLookAndFeel(new SubstanceRavenGraphiteGlassLookAndFeel());
            JFrame.setDefaultLookAndFeelDecorated(true);
            JDialog.setDefaultLookAndFeelDecorated(true);
        } catch (UnsupportedLookAndFeelException ex) {
            Logger.getLogger(NetProbeFrame.class.getName()).log(Level.SEVERE, null, ex);
        }
        probe = new NetProbe();
        frame = new NetProbeFrame(probe);
        frame.setResizable(false);
        frame.setVisible(true);
    }
    
    private void createAddr(){
        String host = frame.getHostname();
        int port = frame.getPort();
        servAddr = new InetSocketAddress(host, port);
    }
    
    public void start(){
        PacketSize = frame.getPacketSize();
        SendingRate = frame.getSendingRate();
        NumPackets = frame.getNumPackets();
        
        if(frame.getProtocol()){
            createAddr();
            Thread tcpThread = new threadTCP(this);
            tcpThread.run();
        }
    }

    public void run() {
        updateUI();
    }
    
    private void updateUI(){
        
    }
}

class threadTCP extends Thread {
    
    private NetProbe m_probe;
    
    threadTCP(NetProbe probe){
        m_probe = probe;
    }
    

    @Override
    public void run() {
        SocketChannel channel = null;
        int ret;
        try {
            channel = SocketChannel.open();
            channel.connect(m_probe.servAddr);

            // Retrieve handle to the embedded socket object:
            // Socket s = channel.socket();

            ByteBuffer buf = ByteBuffer.allocate(12);

            buf.put(0, (byte)(m_probe.PacketSize & 0xff));
            buf.put(1, (byte)(m_probe.PacketSize >> 8 & 0xff));
            buf.put(2, (byte)(m_probe.PacketSize >> 16 & 0xff));
            buf.put(3, (byte)(m_probe.PacketSize >> 24 & 0xff));
            buf.put(4, (byte)(m_probe.SendingRate & 0xff));
            buf.put(5, (byte)(m_probe.SendingRate >> 8 & 0xff));
            buf.put(6, (byte)(m_probe.SendingRate >> 16 & 0xff));
            buf.put(7, (byte)(m_probe.SendingRate >> 24 & 0xff));
            buf.put(8, (byte)(m_probe.NumPackets & 0xff));
            buf.put(9, (byte)(m_probe.NumPackets >> 8 & 0xff));
            buf.put(10, (byte)(m_probe.NumPackets >> 16 & 0xff));
            buf.put(11, (byte)(m_probe.NumPackets >> 24 & 0xff));

            channel.write(buf);
            
            buf = ByteBuffer.allocate(m_probe.PacketSize);
            
            while(true){
                ret = channel.read(buf);
                
                m_probe.bytesTransferred += ret;
                m_probe.packetsTransferred++;
                m_probe.maxPacketNum++;
            }
            
        } catch (IOException ex) {
            Logger.getLogger(threadTCP.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    
}

class threadUDP {
    
    public void run() {
        
    }
    
}
