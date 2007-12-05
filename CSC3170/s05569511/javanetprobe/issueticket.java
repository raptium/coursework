/*
 * issueticket.java
 *
 * Created on November 28, 2007, 5:11 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package javanetprobe;
import com.sun.org.apache.bcel.internal.generic.CALOAD;
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
import java.text.SimpleDateFormat;
/**
 *
 * @author yjwan6
 */
public class issueticket {
    JTextArea jTextArea;
    /** Creates a new instance of issueticket */
    public issueticket(JTextArea jTextArea){
        this.jTextArea=jTextArea;
    }
    
    
    public void directissue(int flightid,int bookingid) throws SQLException{
        connect connectionobject = new connect();
        float fee=0;
        ResultSet rs;
        int seat1=-1;
        String From=null;
        String To=null;
        long departafter=-1;
        long arrivalbefore=-1;
        
        String From_flight=null;
        String To_flight=null;
        
        long depart_flight=-1;
        long arrival_flight=-1;
        
        //from_city varchar(25), to_city varchar(25   depart_after number(20,0),arrival_before
        rs=connectionobject.DoConnect("select bid from books where bid=" + bookingid);
        int ind=0;
        while(rs.next()){
            ind++;
        }
        if(ind>0){
            this.jTextArea.append("\n\nTicket for this booking have been iussed\n\n");
        }
        else
        {
            rs=connectionobject.DoConnect("select from_city,to_city,depart_after,arrival_before from booking where id=" + bookingid);
            int a=0;
            while(rs.next()){
                a++;
                From=rs.getString(1);
                To=rs.getString(2);
                departafter=rs.getLong(3);
                arrivalbefore=rs.getLong(4);
            }
            if(a>0){
                rs=connectionobject.DoConnect("select depart_city, arrival_city ,depart_time ,arrival_time from flight where id="+flightid);
                while(rs.next()){
                    From_flight=rs.getString(1);
                    To_flight=rs.getString(2);
                    depart_flight=rs.getLong(3);
                    arrival_flight=rs.getLong(4);
                }

                if(From.compareTo(From_flight)==0&&To.compareTo(To_flight)==0&&departafter<depart_flight&&arrivalbefore>arrival_flight){
                    rs=connectionobject.DoConnect("select num_seats from flight where id="+flightid);

                    while(rs.next()){
                        seat1=rs.getInt(1);
                    }
                    if(seat1>0){
                        rs=connectionobject.DoConnect("select price from flight where id="+flightid);

                        while(rs.next()){
                            fee=rs.getFloat(1)+20;
                        }
                        
                        int seat=0;
                        rs=connectionobject.DoConnect("select num_seats from flight where id="+flightid);
                            while(rs.next()){
                            seat=rs.getInt(1);
                        }
                        
                        if(seat==0){
                             this.jTextArea.append("\nNo Seat for flight : "+flightid+"\n" );
                        }else{
                            seat--;
                            PreparedStatement updateSales=connectionobject.con.prepareStatement("update flight set num_seats= ? where id=?");
                            updateSales.setInt(1,seat);
                            updateSales.setInt(2,flightid);
                            updateSales.executeUpdate();

                            Calendar cal = Calendar.getInstance();
                            updateSales = connectionobject.con.prepareStatement("insert into books values(?,?,?,?)");
                            updateSales.setInt(1,bookingid); 
                            updateSales.setInt(2, flightid); 
                            updateSales.setLong(3,cal.getTimeInMillis());
                            updateSales.setFloat(4,fee);
                            updateSales.executeUpdate();
                            String tempflightid=new Integer(flightid).toString();
                            String tempbookingid=new Integer(bookingid).toString();
                            this.jTextArea.append("\n\n\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\Ticket Iussed\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n");
                            this.jTextArea.append("\nTicket1:\nBooking ID:\t"+tempbookingid+"\nFlight No:\t"+tempflightid+"\n");
                        }
                    }
                    else{
                        this.jTextArea.append("\nNo Seat for flight : \n"+flightid+"\n" );
                    }
                }
                else{
                    this.jTextArea.append("\n\nBooking ID doesn't match with flight NO\n\n");
                }
            }
            else{
                this.jTextArea.append("\n\nNo such booking\n\n");
            }  
        }
    }
    public void tranissue(int flightid1,int flightid2,int bookingid)throws SQLException{
        connect connectionobject = new connect();
        float fee=0;
        float fee2=0;
        ResultSet rs;
        int seat1=0,seat2=0;
        
        rs=connectionobject.DoConnect("select bid from books where bid=" + bookingid);
        int ind=0;
        while(rs.next()){
            ind++;
        }
        if(ind>0){
            this.jTextArea.append("\n\nTicket for this booking have been issued\n\n");
        }
        else{
            rs=connectionobject.DoConnect("select num_seats from flight where id="+flightid1);
            while(rs.next()){
                seat1=rs.getInt(1);
            }
            rs=connectionobject.DoConnect("select num_seats from flight where id="+flightid2);

            while(rs.next()){
                seat2=rs.getInt(1);
            }
            if(seat1>0&&seat2>0){
                seat1--;
                seat2--;

                PreparedStatement updateSales=connectionobject.con.prepareStatement("update flight set num_seats= ? where id=?");
                updateSales.setInt(1,seat1);
                updateSales.setInt(2,flightid1);
                updateSales.executeUpdate();

                updateSales=connectionobject.con.prepareStatement("update flight set num_seats= ? where id=?");
                updateSales.setInt(1,seat2);
                updateSales.setInt(2,flightid2);
                updateSales.executeUpdate();

                rs=connectionobject.DoConnect("select price from flight where id="+flightid2);
                while(rs.next()){
                    fee=rs.getFloat(1)+20;    
                }

                Calendar cal = Calendar.getInstance();
                updateSales = connectionobject.con.prepareStatement("insert into books values(?,?,?,?)");
                updateSales.setInt(1,bookingid); 
                updateSales.setInt(2, flightid1); 
                updateSales.setLong(3,cal.getTimeInMillis());
                updateSales.setFloat(4,fee);
                updateSales.executeUpdate();


                rs=connectionobject.DoConnect("select  price from flight where id="+flightid2);

                while(rs.next()){
                    fee=rs.getFloat(1)+20;
                }

                cal = Calendar.getInstance();
                updateSales = connectionobject.con.prepareStatement("insert into books values(?,?,?,?)");
                updateSales.setInt(1,bookingid); 
                updateSales.setInt(2, flightid2); 
                updateSales.setLong(3,cal.getTimeInMillis());
                updateSales.setFloat(4,fee);
                updateSales.executeUpdate();

                String tempflightid=new Integer(flightid1).toString();
                String tempflightid2=new Integer(flightid2).toString();
                String tempbookingid=new Integer(bookingid).toString();
                this.jTextArea.append("\n\n\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\Ticket Iussed\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n");
                this.jTextArea.append("\nTicket1:\nBooking ID:\t"+tempbookingid+"\nFlight No:\t"+tempflightid+"\n");
                this.jTextArea.append("\nTicket2:\nBooking ID:\t"+tempbookingid+"\nFlight No:\t"+tempflightid2+"\n");

            }

            else if(seat1>0){
                this.jTextArea.append("\nNo Seat for flight : "+flightid2+"\n" );
            }
            else if(seat2>0){
                 this.jTextArea.append("\nNo Seat for flight : "+flightid1+"\n" );
            }
            else {
                this.jTextArea.append("\nNo Seat for flight : "+flightid1+"\t"+flightid2 );
            }




        }
    }
   
}
