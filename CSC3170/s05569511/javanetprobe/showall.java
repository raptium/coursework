/*
 * showall.java
 *
 * Created on November 28, 2007, 9:57 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package javanetprobe;
import com.sun.org.apache.xalan.internal.xsltc.compiler.Template;
import java.nio.channels.DatagramChannel;
import java.nio.channels.SocketChannel;
import javax.swing.*;
import java.nio.*;
import java.io.*;
import java.net.*;
import java.util.*;
import java.lang.*;
import java.sql.*;
import java.text.SimpleDateFormat;
import java.text.ParseException;   
/**
 *
 * @author yjwan6
 */
public class showall {
    JTextArea jTextArea;
    /** Creates a new instance of showall */
    public showall(JTextArea jTextArea) throws SQLException {
        this.jTextArea=jTextArea;
        connect ConnectObject=new connect();
        ResultSet rs=ConnectObject.DoConnect("select * from flight");
        Flight flight=new Flight();
        this.jTextArea.append("\\\\\\\\\\\\\\\\\\\\Flight\\\\\\\\\\\\\\\\\\\\\n");
        while(rs.next()){
            flight.id=rs.getInt(1);
            flight.airline=rs.getString(2);
            flight.depart_city=rs.getString(3);
            flight.arrival_city=rs.getString(4);
            flight.depart_time=rs.getLong(5);
            flight.arrival_time=rs.getLong(6);
            flight.price=rs.getFloat(7);
            flight.num_seats=rs.getInt(8);
            String temp=flight.toString2();
            this.jTextArea.append(temp+"\n");
        }
        
        
        rs=ConnectObject.DoConnect("select * from booking");
        Booking booking=new Booking();
        this.jTextArea.append("\n\\\\\\\\\\\\\\\\\\\\Booking\\\\\\\\\\\\\\\\\\\\\n");
        while(rs.next()){
            booking.id=rs.getInt(1);
            booking.p_name=rs.getString(2);
            booking.frm_city=rs.getString(3);
            booking.to_city=rs.getString(4);
            booking.depart_after=rs.getLong(5);
            booking.arrival_before=rs.getLong(6);
            booking.p_phone=rs.getString(7);
            booking.p_ccno=rs.getString(8);
            String temp=booking.toString();
            this.jTextArea.append(temp+"\n");
        }
        rs=ConnectObject.DoConnect("select * from books");
        Books books=new Books();
        this.jTextArea.append("\n\\\\\\\\\\\\\\\\\\\\Books\\\\\\\\\\\\\\\\\\\\\n");
        while(rs.next()){
            books.bid=rs.getInt(1);
            books.fid=rs.getInt(2);
            books.issue_time=rs.getLong(3);
            books.fee=rs.getFloat(4);
            String temp=books.toString();
            this.jTextArea.append(temp+"\n");
        }
        ConnectObject.CloseConnection();
        
    }    
}
