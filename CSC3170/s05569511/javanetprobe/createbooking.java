/*
 * createbooking.java
 *
 * Created on November 28, 2007, 2:34 PM
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
public class createbooking {
    
    String From;
    String To;
    String SortMethod;
    String Dyear;
    String Dmonth;
    String Dday;
    String Dtime;
    String Ayear;
    String Amonth;
    String Aday;
    String Atime;
    JTextArea jTextArea;
    String CardNo;
    String PhoneNo;
    String Name;
   // String SortMethod;

    String Departtime;
    String Arrivaltime;
    long Departtime_long;
    long Arrivaltime_long;
    connect ConnectObject;
    SimpleDateFormat df;
    int bookingID;
    public createbooking() {
         ConnectObject=new connect();
         df = new SimpleDateFormat("yyyy-MM-dd  HH:mm");   
        
    }
    public void createbookings() throws ParseException, SQLException{
             this.Departtime=(Dyear+"-"+Dmonth+"-"+Dday+"  "+Dtime+":00");
         this.Arrivaltime=(Ayear+"-"+Amonth+"-"+Aday+"  "+Atime+":00");
         this.Arrivaltime_long=df.parse(this.Arrivaltime).getTime();
         this.Departtime_long=df.parse(this.Departtime).getTime();
    
      
        ResultSet rs=this.ConnectObject.DoConnect("select count(*) from booking");
        while(rs.next()){
             bookingID=rs.getInt(1)+1;
        }
        PreparedStatement updateSales = ConnectObject.con.prepareStatement("insert into booking values(?,?,?,?,?,?,?,?)");
        updateSales.setInt(1,this.bookingID); 
        updateSales.setString(2,this.Name); 
        updateSales.setString(3,this.From);
        updateSales.setString(4,this.To);
        updateSales.setLong(5,this.Departtime_long);
        updateSales.setLong(6,this.Arrivaltime_long);
        updateSales.setString(7,this.PhoneNo);
        updateSales.setString(8,this.CardNo);
        updateSales.executeUpdate();
        this.jTextArea.append("\n\nCreated a Booking successfully:\n" +
                "ID: "+bookingID+"\tPassenger: "+this.Name+
                "\n"+this.From+" - "+this.To+"\n"+
                this.Departtime+" - "+this.Arrivaltime+
                "\nPhoneNo: "+this.PhoneNo+"\nCredit Card: "+this.CardNo);
    } 
}
