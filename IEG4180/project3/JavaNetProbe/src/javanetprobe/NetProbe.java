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
//import org.jvnet.substance.*;
//import org.jvnet.substance.skin.*;





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
    
    protected Thread m_TimerThread;
    protected Thread m_WorkerThread;
    
    protected long m_startTime;
    
    
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
            UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
            JFrame.setDefaultLookAndFeelDecorated(true);
            JDialog.setDefaultLookAndFeelDecorated(true);
        } catch (ClassNotFoundException ex) {
            Logger.getLogger(NetProbe.class.getName()).log(Level.SEVERE, null, ex);
        } catch (InstantiationException ex) {
            Logger.getLogger(NetProbe.class.getName()).log(Level.SEVERE, null, ex);
        } catch (IllegalAccessException ex) {
            Logger.getLogger(NetProbe.class.getName()).log(Level.SEVERE, null, ex);
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
    
    public void connect(){
        PacketSize = frame.getPacketSize();
        SendingRate = frame.getSendingRate();
        NumPackets = frame.getNumPackets();
        
        createAddr();
        if(frame.getProtocol()){
            m_WorkerThread = new threadTCP(this);
        }else{
            m_WorkerThread = new threadUDP(this);
        }
        m_WorkerThread.start();
        m_TimerThread = new TimerThread(100, this);
        m_TimerThread.start();
        m_startTime = System.currentTimeMillis();
    }
    
    public void stop() {
        if(frame.getProtocol()) {
            ((threadTCP)m_WorkerThread).Quit();
        } else {
            ((threadUDP)m_WorkerThread).Quit();
        }
        ((TimerThread)m_TimerThread).QuitUpdate();

        bytesTransferred = 0;
        packetsTransferred = 0;
        maxPacketNum = -1;
        frame.resetButton();
    }

    public void run() {
        updateUI();
    }
    
    private void updateUI(){
        frame.setPacketsTransferred(packetsTransferred);
        frame.setPacketLoss(100.0 - packetsTransferred / (maxPacketNum + 1.0) * 100.0);
        frame.setNumPacketLoss(maxPacketNum + 1 - packetsTransferred);
        frame.setTransferringRate(bytesTransferred / (System.currentTimeMillis() - m_startTime) * 1000.0);
        frame.setTimeElapsed((System.currentTimeMillis() - m_startTime) / 1000.0);
    }
}

class threadTCP extends Thread {
    
    private NetProbe m_probe;
    protected boolean m_bToQuit;
    
    threadTCP(NetProbe probe){
        m_probe = probe;
        m_bToQuit = false;
    }
    
    public void Quit() { 
        m_bToQuit = true;
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
            
            while(!m_bToQuit){
                buf.clear();
                ret = channel.read(buf);
                
                m_probe.bytesTransferred += ret;
                m_probe.packetsTransferred++;
                m_probe.maxPacketNum++;
            }
            m_probe.stop();
            channel.close();
        } catch (IOException ex) {
            Logger.getLogger(threadTCP.class.getName()).log(Level.SEVERE, null, ex);
        }
        
    }
    
}

class threadUDP extends Thread{
    
    private NetProbe m_probe;
    protected boolean m_bToQuit;
    
    threadUDP(NetProbe probe){
        m_probe = probe;
        m_bToQuit = false;
    }
    
    public void Quit() { 
        m_bToQuit = true;
    }
    
    public final static int swabInt(int v) {
        return  (v >>> 24) | (v << 24) | 
          ((v << 8) & 0x00FF0000) | ((v >> 8) & 0x0000FF00);
    }

    @Override
    public void run() {
        DatagramChannel channel = null;
        int ret;
        try {
            channel = DatagramChannel.open();
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
            
            int maxNum;
            while(!m_bToQuit){
                buf.clear();
                ret = channel.read(buf);
                maxNum = buf.getInt(0);
                maxNum = swabInt(maxNum);
                
                m_probe.bytesTransferred += ret;
                m_probe.packetsTransferred++;
                m_probe.maxPacketNum = maxNum;
            }
            m_probe.stop();
            channel.close();
        } catch (IOException ex) {
            Logger.getLogger(threadTCP.class.getName()).log(Level.SEVERE, null, ex);
        }
        
    }
    
    
}


class TimerThread extends Thread {
    protected int m_iRefreshInterval;
    protected Runnable m_UpdateCode;
    protected boolean m_bToQuit = false;
    
    
    // Constructor
    TimerThread(int refresh_interval, Runnable update_code) {
        m_iRefreshInterval = refresh_interval;
        m_UpdateCode = update_code;
        m_bToQuit = false;
    }
    
    // Set Function
    public void QuitUpdate() { 
        m_bToQuit = true;
    }
    
    // Thread entry function
    @Override
    public void run() {
        // Generates one event to m_UpdateCode every m_RefreshInterval milliseconds
        while (!m_bToQuit) {
            javax.swing.SwingUtilities.invokeLater(m_UpdateCode);
            try {
                sleep(m_iRefreshInterval);
            } catch (Exception ex) {
                Logger.getLogger(threadTCP.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }
}
