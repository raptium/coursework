/*CSC3170B
 *GROUP ONE
 *WAN YUJIE
 *GUAN HAO
 *
 *
 *
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


class Main {
    
    public javaClientFrame myFrame;
    
    public Main()
    {
       myFrame =  new javaClientFrame();
       myFrame.setTitle("Flight Booking System");
       myFrame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
       myFrame.pack();
       myFrame.setVisible(true);
       
    }
    
    public static void main(String[] args) 
         {
             
             
             Main myMain = new Main();

 
             
         }   
   
    
}

 class Search 
    {
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
        
        String Departtime;
        String Arrivaltime;
        long Departtime_long;
        long Arrivaltime_long;
        connect ConnectObject;
        SimpleDateFormat df;
        List Onelist = new LinkedList();   
        List TranList=new LinkedList();
        List OnlistBackup=new LinkedList();   
        List TranListBackup=new LinkedList();

        public Search()
         {
                 ConnectObject=new connect();
                 df = new SimpleDateFormat("yyyy-MM-dd  HH:mm");
                 Onelist = new ArrayList();          
                 TranList=new ArrayList();    
                 OnlistBackup=new ArrayList();
                 TranListBackup=new ArrayList();
         }
         public void processresult(){
             try{
                 this.Departtime=(Dyear+"-"+Dmonth+"-"+Dday+"  "+Dtime+":00");
                 this.Arrivaltime=(Ayear+"-"+Amonth+"-"+Aday+"  "+Atime+":00");
                 this.Arrivaltime_long=df.parse(this.Arrivaltime).getTime();
                 this.Departtime_long=df.parse(this.Departtime).getTime();
                 
                 this.ConnectObject.DoConnect("drop view tempflight");
                 this.ConnectObject.DoConnect("drop view temp");
                 this.ConnectObject.DoConnect("drop view temp2");
                 this.ConnectObject.DoConnect("drop view temp5");
                 this.ConnectObject.DoConnect("drop view temp6");
                 
                 this.ConnectObject.DoConnect("create view tempflight as select * from flight");
                 this.ConnectObject.DoConnect("create view temp (fid1,fid2) as select a.id,b.id from tempflight a, flight b where a.depart_city='"+this.From+"' and b.arrival_city='"+this.To+"'"+"and a.depart_time >"+this.Departtime_long+"and b.arrival_time<" +this.Arrivaltime_long+"and a.arrival_time <" +this.Arrivaltime_long);
                 this.ConnectObject.DoConnect("create view temp2 (fid1,fid2) as select a.fid1,a.fid2 from temp a, flight b, tempflight c where a.fid1=b.id and a.fid2=c.id and  (b.arrival_city=c.depart_city and b.arrival_time<c.depart_time)");
                 //ConnectObject.DoConnect("create view temp3(fid1,fid2) as select * from temp2 where fid1=fid2 ");
                 //ConnectObject.DoConnect("create view temp4 as select distinct temp2.fid1, temp2.fid2 from temp2 , temp3 where temp3.fid1!=temp2.fid1 and temp3.fid1!=temp2.fid2");
                 this.ConnectObject.DoConnect("create view temp5 (fid1,fid2,duration,price) as select a.fid1,a.fid2,c.arrival_time-b.depart_time,b.price+c.price from temp2 a,flight b,tempflight  c where b.id=a.fid1 and c.id=a.fid2");
                 this.ConnectObject.DoConnect("create view temp6 (fid1,duration,price) as select a.id,a.arrival_time-a.depart_time,a.price from  flight a where a.depart_city='"+this.From+"' and a.arrival_city='"+this.To+"' and a.depart_time >"+this.Departtime_long+"and a.arrival_time < "+this.Arrivaltime_long);
                 this.serchprocess();
                 ConnectObject.CloseConnection();
           
              //   this.ConnectObject.DoConnect("drop view tempflight");
             //   this.ConnectObject.DoConnect("drop view temp");
              //   this.ConnectObject.DoConnect("drop view temp2");
              //   this.ConnectObject.DoConnect("drop view temp5");
               //  this.ConnectObject.DoConnect("drop view temp6");

         }catch(ParseException e){
             //this.jTextArea.append("here:  "+e.toString());
             return;
       }
 }
         private void serchprocess(){
             try{
                this.jTextArea.append("\n\n/////////////////////////////////Search Start/////////////////////////////////\n");
                 this.jTextArea.setEditable(true);
         
                if(SortMethod.compareTo("fee")==0){
                    ResultSet rs=ConnectObject.DoConnect("select fid1,price from temp6 order by price");
                    while(rs.next()){
                        temprecoddirect record= new temprecoddirect(rs.getInt(1),0,(rs.getFloat(2)+20));
                        this.Onelist.add(record);
                        this.OnlistBackup.add(record);
                    }
                    rs=ConnectObject.DoConnect("select fid1,fid2,price from temp5 order by price");
                    while(rs.next()){
                        temprecodtran record= new temprecodtran(rs.getInt(1),rs.getInt(2),0,(rs.getFloat(3)+40));
                        this.TranList.add(record);
                        this.TranListBackup.add(record);
                    }
                
                    this.printout(1);
                }
                else{
                    ResultSet rs=ConnectObject.DoConnect("select fid1,duration,price from temp6 order by duration");
                    while(rs.next()){
                        temprecoddirect record= new temprecoddirect(rs.getInt(1),rs.getLong(2),(rs.getFloat(3)+20));
                        this.Onelist.add(record);
                    }
                    rs=ConnectObject.DoConnect("select fid1,fid2,duration,price from temp5 order by duration");
                    while(rs.next()){
                        temprecodtran record= new temprecodtran(rs.getInt(1),rs.getInt(2),rs.getLong(3),(rs.getFloat(4)+40));
                        this.TranList.add(record);
                    }
                
                    this.printout(2);
                }
                
             }catch(SQLException e){
                // this.jTextArea.append(e.toString());
                 return;
             }   
         }
         
         private void printout(int i){
             try{
                 int directsize=this.Onelist.size();
                 int transize=this.TranList.size();
                 temprecoddirect temp1=null;
                 temprecodtran temp2=null;
                 Flight flight1=new Flight();
                 Flight flight2=new Flight();
                 Flight flight=new Flight();
                 int NoOfChoice=1;
                 ResultSet rs;
                 if(directsize>0){
                    temp1=(temprecoddirect)this.Onelist.get(0);
                 }
                 if(transize>0){
                    temp2=(temprecodtran)this.TranList.get(0);
                 }
                 
                 if(i==1){
                     if(directsize>0&&transize>0){
                         while(directsize>0&&transize>0){
                            if(temp1.price>temp2.price){
                                rs=this.ConnectObject.DoConnect("select * from flight where flight.id="+temp2.id1);
                                while(rs.next()){
                                    flight1.id=rs.getInt(1);
                                    flight1.airline=rs.getString(2);
                                    flight1.depart_city=rs.getString(3);
                                    flight1.arrival_city=rs.getString(4);
                                    flight1.depart_time=rs.getLong(5);
                                    flight1.arrival_time=rs.getLong(6);
                                    flight1.price=rs.getFloat(7);
                                    flight1.num_seats=rs.getInt(8);
                                }
                                rs=this.ConnectObject.DoConnect("select * from flight where flight.id="+temp2.id2);
                                while(rs.next()){
                                    flight2.id=rs.getInt(1);
                                    flight2.airline=rs.getString(2);
                                    flight2.depart_city=rs.getString(3);
                                    flight2.arrival_city=rs.getString(4);
                                    flight2.depart_time=rs.getLong(5);
                                    flight2.arrival_time=rs.getLong(6);
                                    flight2.price=rs.getFloat(7);
                                    flight2.num_seats=rs.getInt(8);
                                }
                               this.jTextArea.append("\n\n"+"Itinerary "+new Integer(NoOfChoice).toString()+":(" 
                                +flight1.dtoStringTime()+ " - " +flight2.atoStringTime()+ ", " + "Fee: " +new Float(temp2.price).toString()+" $"+", Duration: " + new Float((new Float(flight2.arrival_time-flight1.depart_time))/1000/3600).toString() +"hrs"+
                                "):\n" + "Flight  #" +new Integer(flight1.id).toString() + "("+ flight1.depart_city + " - "+ flight1.arrival_city +"), Flight  #"+new Integer(flight2.id).toString()
                                +"("+ flight2.depart_city + " - "+ flight2.arrival_city +")\n" 
                                +"Flight #" + new Integer(flight1.id).toString() +"'s info:\n"+
                                flight1.toString()
                                +"\nFlight #" + new Integer(flight2.id).toString() +"'s info:\n"+
                                flight2.toString());
                                if(this.TranList.size()>0){
                                    this.TranList.remove(0);
                                    if(this.TranList.size()>0)
                                        temp2=(temprecodtran)this.TranList.get(0);

                                }
                                transize--;
                                 NoOfChoice++;
                            }
                            else{
                                rs=this.ConnectObject.DoConnect("select * from flight where flight.id="+temp1.id);
                                while(rs.next()){
                                    flight.id=rs.getInt(1);
                                    flight.airline=rs.getString(2);
                                    flight.depart_city=rs.getString(3);
                                    flight.arrival_city=rs.getString(4);
                                    flight.depart_time=rs.getLong(5);
                                    flight.arrival_time=rs.getLong(6);
                                    flight.price=rs.getFloat(7);
                                    flight.num_seats=rs.getInt(8);
                                }
                                this.jTextArea.append("\n\n"+"Itinerary "+new Integer(NoOfChoice).toString()+":(" 
                                +flight.dtoStringTime()+ " - " +flight.atoStringTime()+ ", " + "Fee: " +new Float(temp1.price).toString()+" $"+", Duration: " + new Float(new Float(flight.arrival_time-flight.depart_time)/1000/3600).toString() +"hrs"+
                                "):\n" + "Flight  #" +new Integer(flight.id).toString() + "("+ flight.depart_city + " - "+ flight.arrival_city +") \n" 
                                +"Flight #" + new Integer(flight.id).toString() +"'s info:\n"+
                                flight.toString());     
                                if(this.Onelist.size()>0){

                                    this.Onelist.remove(0);
                                    if(this.Onelist.size()>0)
                                        temp1=(temprecoddirect)this.Onelist.get(0);


                                }
                                 directsize--;
                            NoOfChoice++;
                         }

                         if(directsize==0&&transize>0){
                             while(transize>0){
                                 rs=this.ConnectObject.DoConnect("select * from flight where flight.id="+temp2.id1);
                                while(rs.next()){
                                    flight1.id=rs.getInt(1);
                                    flight1.airline=rs.getString(2);
                                    flight1.depart_city=rs.getString(3);
                                    flight1.arrival_city=rs.getString(4);
                                    flight1.depart_time=rs.getLong(5);
                                    flight1.arrival_time=rs.getLong(6);
                                    flight1.price=rs.getFloat(7);
                                    flight1.num_seats=rs.getInt(8);
                                }
                                rs=this.ConnectObject.DoConnect("select * from flight where flight.id="+temp2.id2);
                                while(rs.next()){
                                    flight2.id=rs.getInt(1);
                                    flight2.airline=rs.getString(2);
                                    flight2.depart_city=rs.getString(3);
                                    flight2.arrival_city=rs.getString(4);
                                    flight2.depart_time=rs.getLong(5);
                                    flight2.arrival_time=rs.getLong(6);
                                    flight2.price=rs.getFloat(7);
                                    flight2.num_seats=rs.getInt(8);
                                }
                                this.jTextArea.append("\n\n"+"Itinerary "+new Integer(NoOfChoice).toString()+":(" 
                                +flight1.dtoStringTime()+ " - " +flight2.atoStringTime()+ ", " + "Fee: " +new Float(temp2.price).toString()+" $"+", Duration: " + new Float(new Float(flight2.arrival_time-flight1.depart_time)/1000/3600).toString() +"hrs"+
                                "):\n" + "Flight  #" +new Integer(flight1.id).toString() + "("+ flight1.depart_city + " - "+ flight1.arrival_city +"), Flight  #"+new Integer(flight2.id).toString()
                                +"("+ flight2.depart_city + " - "+ flight2.arrival_city +")\n" 
                                +"Flight #" + new Integer(flight1.id).toString() +"'s info:\n"+
                                flight1.toString()
                                +"\nFlight #" + new Integer(flight2.id).toString() +"'s info:\n"+
                                flight2.toString());
                                if(this.TranList.size()>0){
                                    this.TranList.remove(0);
                                    if(this.TranList.size()>0)
                                        temp2=(temprecodtran)this.TranList.get(0);
                                }
                                transize--;
                                NoOfChoice++;
                             }
                         }

                         else if(directsize>0&&transize==0){
                             while(directsize>0){
                                rs=this.ConnectObject.DoConnect("select * from flight where flight.id="+temp1.id);
                                while(rs.next()){
                                    flight.id=rs.getInt(1);
                                    flight.airline=rs.getString(2);
                                    flight.depart_city=rs.getString(3);
                                    flight.arrival_city=rs.getString(4);
                                    flight.depart_time=rs.getLong(5);
                                    flight.arrival_time=rs.getLong(6);
                                    flight.price=rs.getFloat(7);
                                    flight.num_seats=rs.getInt(8);
                                }
                               this.jTextArea.append("\n\n"+"Itinerary "+new Integer(NoOfChoice).toString()+":(" 
                                +flight.dtoStringTime()+ " - " +flight.atoStringTime()+ ", " + "Fee: " +new Float(temp1.price).toString()+" $"+", Duration: " + new Float(new Float(flight.arrival_time-flight.depart_time)/1000/3600).toString() +"hrs"+
                                "):\n" + "Flight  #" +new Integer(flight.id).toString() + "("+ flight.depart_city + " - "+ flight.arrival_city +") \n" 
                                +"Flight #" + new Integer(flight.id).toString() +"'s info:\n"+
                                flight.toString());     
                                if(this.Onelist.size()>0){
                                    this.Onelist.remove(0);
                                    if(this.Onelist.size()>0)
                                        temp1=(temprecoddirect)this.Onelist.get(0);
                                }
                                directsize--;
                                NoOfChoice++;


                             }
                         }
                         }
                     }

                     else if(directsize>0&&transize==0){
                         while(directsize>0){
                                rs=this.ConnectObject.DoConnect("select * from flight where flight.id="+temp1.id);
                                while(rs.next()){
                                    flight.id=rs.getInt(1);
                                    flight.airline=rs.getString(2);
                                    flight.depart_city=rs.getString(3);
                                    flight.arrival_city=rs.getString(4);
                                    flight.depart_time=rs.getLong(5);
                                    flight.arrival_time=rs.getLong(6);
                                    flight.price=rs.getFloat(7);
                                    flight.num_seats=rs.getInt(8);
                                }
                               this.jTextArea.append("\n\n"+"Itinerary "+new Integer(NoOfChoice).toString()+":(" 
                                +flight.dtoStringTime()+ " - " +flight.atoStringTime()+ ", " + "Fee: " +new Float(temp1.price).toString()+" $"+", Duration: " + new Float(new Float(flight.arrival_time-flight.depart_time)/1000/3600).toString() +"hrs"+
                                "):\n" + "Flight  #" +new Integer(flight.id).toString() + "("+ flight.depart_city + " - "+ flight.arrival_city +") \n" 
                                +"Flight #" + new Integer(flight.id).toString() +"'s info:\n"+
                                flight.toString());     
                                if(this.Onelist.size()>0){
                                    this.Onelist.remove(0);
                                    if(this.Onelist.size()>0)
                                        temp1=(temprecoddirect)this.Onelist.get(0);
                                }
                                directsize--;
                                NoOfChoice++;
                         }
                     }

                     else if(directsize==0&&transize>0){
                         while(transize>0){
                                 rs=this.ConnectObject.DoConnect("select * from flight where flight.id="+temp2.id1);
                                while(rs.next()){
                                    flight1.id=rs.getInt(1);
                                    flight1.airline=rs.getString(2);
                                    flight1.depart_city=rs.getString(3);
                                    flight1.arrival_city=rs.getString(4);
                                    flight1.depart_time=rs.getLong(5);
                                    flight1.arrival_time=rs.getLong(6);
                                    flight1.price=rs.getFloat(7);
                                    flight1.num_seats=rs.getInt(8);
                                }
                                rs=this.ConnectObject.DoConnect("select * from flight where flight.id="+temp2.id2);
                                while(rs.next()){
                                    flight2.id=rs.getInt(1);
                                    flight2.airline=rs.getString(2);
                                    flight2.depart_city=rs.getString(3);
                                    flight2.arrival_city=rs.getString(4);
                                    flight2.depart_time=rs.getLong(5);
                                    flight2.arrival_time=rs.getLong(6);
                                    flight2.price=rs.getFloat(7);
                                    flight2.num_seats=rs.getInt(8);
                                }
                                this.jTextArea.append("\n\n"+"Itinerary "+new Integer(NoOfChoice).toString()+":(" 
                                +flight1.dtoStringTime()+ " - " +flight2.atoStringTime()+ ", " + "Fee: " +new Float(temp2.price).toString()+" $"+", Duration: " + new Float(new Float(flight2.arrival_time-flight1.depart_time)/1000/3600).toString() +"hrs"+
                                "):\n" + "Flight  #" +new Integer(flight1.id).toString() + "("+ flight1.depart_city + " - "+ flight1.arrival_city +"), Flight  #"+new Integer(flight2.id).toString()
                                +"("+ flight2.depart_city + " - "+ flight2.arrival_city +")\n" 
                                +"Flight #" + new Integer(flight1.id).toString() +"'s info:\n"+
                                flight1.toString()
                                +"\nFlight #" + new Integer(flight2.id).toString() +"'s info:\n"+
                                flight2.toString());
                                if(this.TranList.size()>0){
                                    this.TranList.remove(0);
                                    if(this.TranList.size()>0)
                                        temp2=(temprecodtran)this.TranList.get(0);
                                }
                                transize--;
                                NoOfChoice++;
                             }

                     }

                     else{
                         this.jTextArea.append("\nNot Match\n");
                     }
                 }
                 
                 else{
                     if(directsize>0&&transize>0){
                         while(directsize>0&&transize>0){
                            if(temp1.duration>temp2.duration){
                                rs=this.ConnectObject.DoConnect("select * from flight where flight.id="+temp2.id1);
                                while(rs.next()){
                                    flight1.id=rs.getInt(1);
                                    flight1.airline=rs.getString(2);
                                    flight1.depart_city=rs.getString(3);
                                    flight1.arrival_city=rs.getString(4);
                                    flight1.depart_time=rs.getLong(5);
                                    flight1.arrival_time=rs.getLong(6);
                                    flight1.price=rs.getFloat(7);
                                    flight1.num_seats=rs.getInt(8);
                                }
                                rs=this.ConnectObject.DoConnect("select * from flight where flight.id="+temp2.id2);
                                while(rs.next()){
                                    flight2.id=rs.getInt(1);
                                    flight2.airline=rs.getString(2);
                                    flight2.depart_city=rs.getString(3);
                                    flight2.arrival_city=rs.getString(4);
                                    flight2.depart_time=rs.getLong(5);
                                    flight2.arrival_time=rs.getLong(6);
                                    flight2.price=rs.getFloat(7);
                                    flight2.num_seats=rs.getInt(8);
                                }
                               this.jTextArea.append("\n\n"+"Itinerary "+new Integer(NoOfChoice).toString()+":(" 
                                +flight1.dtoStringTime()+ " - " +flight2.atoStringTime()+ ", " + "Fee: " +new Float(temp2.price).toString()+" $"+", Duration: " + new Float((new Float(flight2.arrival_time-flight1.depart_time))/1000/3600).toString() +"hrs"+
                                "):\n" + "Flight  #" +new Integer(flight1.id).toString() + "("+ flight1.depart_city + " - "+ flight1.arrival_city +"), Flight  #"+new Integer(flight2.id).toString()
                                +"("+ flight2.depart_city + " - "+ flight2.arrival_city +")\n" 
                                +"Flight #" + new Integer(flight1.id).toString() +"'s info:\n"+
                                flight1.toString()
                                +"\nFlight #" + new Integer(flight2.id).toString() +"'s info:\n"+
                                flight2.toString());
                                if(this.TranList.size()>0){
                                    this.TranList.remove(0);
                                    if(this.TranList.size()>0)
                                        temp2=(temprecodtran)this.TranList.get(0);

                                }
                                transize--;
                                 NoOfChoice++;
                            }
                            else{
                                rs=this.ConnectObject.DoConnect("select * from flight where flight.id="+temp1.id);
                                while(rs.next()){
                                    flight.id=rs.getInt(1);
                                    flight.airline=rs.getString(2);
                                    flight.depart_city=rs.getString(3);
                                    flight.arrival_city=rs.getString(4);
                                    flight.depart_time=rs.getLong(5);
                                    flight.arrival_time=rs.getLong(6);
                                    flight.price=rs.getFloat(7);
                                    flight.num_seats=rs.getInt(8);
                                }
                                this.jTextArea.append("\n\n"+"Itinerary "+new Integer(NoOfChoice).toString()+":(" 
                                +flight.dtoStringTime()+ " - " +flight.atoStringTime()+ ", " + "Fee: " +new Float(temp1.price).toString()+" $"+", Duration: " + new Float(new Float(flight.arrival_time-flight.depart_time)/1000/3600).toString() +"hrs"+
                                "):\n" + "Flight  #" +new Integer(flight.id).toString() + "("+ flight.depart_city + " - "+ flight.arrival_city +") \n" 
                                +"Flight #" + new Integer(flight.id).toString() +"'s info:\n"+
                                flight.toString());     
                                if(this.Onelist.size()>0){

                                    this.Onelist.remove(0);
                                    if(this.Onelist.size()>0)
                                        temp1=(temprecoddirect)this.Onelist.get(0);


                                }
                                 directsize--;
                            NoOfChoice++;
                         }

                         if(directsize==0&&transize>0){
                             while(transize>0){
                                 rs=this.ConnectObject.DoConnect("select * from flight where flight.id="+temp2.id1);
                                while(rs.next()){
                                    flight1.id=rs.getInt(1);
                                    flight1.airline=rs.getString(2);
                                    flight1.depart_city=rs.getString(3);
                                    flight1.arrival_city=rs.getString(4);
                                    flight1.depart_time=rs.getLong(5);
                                    flight1.arrival_time=rs.getLong(6);
                                    flight1.price=rs.getFloat(7);
                                    flight1.num_seats=rs.getInt(8);
                                }
                                rs=this.ConnectObject.DoConnect("select * from flight where flight.id="+temp2.id2);
                                while(rs.next()){
                                    flight2.id=rs.getInt(1);
                                    flight2.airline=rs.getString(2);
                                    flight2.depart_city=rs.getString(3);
                                    flight2.arrival_city=rs.getString(4);
                                    flight2.depart_time=rs.getLong(5);
                                    flight2.arrival_time=rs.getLong(6);
                                    flight2.price=rs.getFloat(7);
                                    flight2.num_seats=rs.getInt(8);
                                }
                                this.jTextArea.append("\n\n"+"Itinerary "+new Integer(NoOfChoice).toString()+":(" 
                                +flight1.dtoStringTime()+ " - " +flight2.atoStringTime()+ ", " + "Fee: " +new Float(temp2.price).toString()+" $"+", Duration: " + new Float(new Float(flight2.arrival_time-flight1.depart_time)/1000/3600).toString() +"hrs"+
                                "):\n" + "Flight  #" +new Integer(flight1.id).toString() + "("+ flight1.depart_city + " - "+ flight1.arrival_city +"), Flight  #"+new Integer(flight2.id).toString()
                                +"("+ flight2.depart_city + " - "+ flight2.arrival_city +")\n" 
                                +"Flight #" + new Integer(flight1.id).toString() +"'s info:\n"+
                                flight1.toString()
                                +"\nFlight #" + new Integer(flight2.id).toString() +"'s info:\n"+
                                flight2.toString());
                                if(this.TranList.size()>0){
                                    this.TranList.remove(0);
                                    if(this.TranList.size()>0)
                                        temp2=(temprecodtran)this.TranList.get(0);
                                }
                                transize--;
                                NoOfChoice++;
                             }
                         }

                         else if(directsize>0&&transize==0){
                             while(directsize>0){
                                rs=this.ConnectObject.DoConnect("select * from flight where flight.id="+temp1.id);
                                while(rs.next()){
                                    flight.id=rs.getInt(1);
                                    flight.airline=rs.getString(2);
                                    flight.depart_city=rs.getString(3);
                                    flight.arrival_city=rs.getString(4);
                                    flight.depart_time=rs.getLong(5);
                                    flight.arrival_time=rs.getLong(6);
                                    flight.price=rs.getFloat(7);
                                    flight.num_seats=rs.getInt(8);
                                }
                               this.jTextArea.append("\n\n"+"Itinerary "+new Integer(NoOfChoice).toString()+":(" 
                                +flight.dtoStringTime()+ " - " +flight.atoStringTime()+ ", " + "Fee: " +new Float(temp1.price).toString()+" $"+", Duration: " + new Float(new Float(flight.arrival_time-flight.depart_time)/1000/3600).toString() +"hrs"+
                                "):\n" + "Flight  #" +new Integer(flight.id).toString() + "("+ flight.depart_city + " - "+ flight.arrival_city +") \n" 
                                +"Flight #" + new Integer(flight.id).toString() +"'s info:\n"+
                                flight.toString());     
                                if(this.Onelist.size()>0){
                                    this.Onelist.remove(0);
                                    if(this.Onelist.size()>0)
                                        temp1=(temprecoddirect)this.Onelist.get(0);
                                }
                                directsize--;
                                NoOfChoice++;


                             }
                         }
                         }
                     }

                     else if(directsize>0&&transize==0){
                         while(directsize>0){
                                rs=this.ConnectObject.DoConnect("select * from flight where flight.id="+temp1.id);
                                while(rs.next()){
                                    flight.id=rs.getInt(1);
                                    flight.airline=rs.getString(2);
                                    flight.depart_city=rs.getString(3);
                                    flight.arrival_city=rs.getString(4);
                                    flight.depart_time=rs.getLong(5);
                                    flight.arrival_time=rs.getLong(6);
                                    flight.price=rs.getFloat(7);
                                    flight.num_seats=rs.getInt(8);
                                }
                               this.jTextArea.append("\n\n"+"Itinerary "+new Integer(NoOfChoice).toString()+":(" 
                                +flight.dtoStringTime()+ " - " +flight.atoStringTime()+ ", " + "Fee: " +new Float(temp1.price).toString()+" $"+", Duration: " + new Float(new Float(flight.arrival_time-flight.depart_time)/1000/3600).toString() +"hrs"+
                                "):\n" + "Flight  #" +new Integer(flight.id).toString() + "("+ flight.depart_city + " - "+ flight.arrival_city +") \n" 
                                +"Flight #" + new Integer(flight.id).toString() +"'s info:\n"+
                                flight.toString());     
                                if(this.Onelist.size()>0){
                                    this.Onelist.remove(0);
                                    if(this.Onelist.size()>0)
                                        temp1=(temprecoddirect)this.Onelist.get(0);
                                }
                                directsize--;
                                NoOfChoice++;
                         }
                     }

                     else if(directsize==0&&transize>0){
                         while(transize>0){
                                 rs=this.ConnectObject.DoConnect("select * from flight where flight.id="+temp2.id1);
                                while(rs.next()){
                                    flight1.id=rs.getInt(1);
                                    flight1.airline=rs.getString(2);
                                    flight1.depart_city=rs.getString(3);
                                    flight1.arrival_city=rs.getString(4);
                                    flight1.depart_time=rs.getLong(5);
                                    flight1.arrival_time=rs.getLong(6);
                                    flight1.price=rs.getFloat(7);
                                    flight1.num_seats=rs.getInt(8);
                                }
                                rs=this.ConnectObject.DoConnect("select * from flight where flight.id="+temp2.id2);
                                while(rs.next()){
                                    flight2.id=rs.getInt(1);
                                    flight2.airline=rs.getString(2);
                                    flight2.depart_city=rs.getString(3);
                                    flight2.arrival_city=rs.getString(4);
                                    flight2.depart_time=rs.getLong(5);
                                    flight2.arrival_time=rs.getLong(6);
                                    flight2.price=rs.getFloat(7);
                                    flight2.num_seats=rs.getInt(8);
                                }
                                this.jTextArea.append("\n\n"+"Itinerary "+new Integer(NoOfChoice).toString()+":(" 
                                +flight1.dtoStringTime()+ " - " +flight2.atoStringTime()+ ", " + "Fee: " +new Float(temp2.price).toString()+" $"+", Duration: " + new Float(new Float(flight2.arrival_time-flight1.depart_time)/1000/3600).toString() +"hrs"+
                                "):\n" + "Flight  #" +new Integer(flight1.id).toString() + "("+ flight1.depart_city + " - "+ flight1.arrival_city +"), Flight  #"+new Integer(flight2.id).toString()
                                +"("+ flight2.depart_city + " - "+ flight2.arrival_city +")\n" 
                                +"Flight #" + new Integer(flight1.id).toString() +"'s info:\n"+
                                flight1.toString()
                                +"\nFlight #" + new Integer(flight2.id).toString() +"'s info:\n"+
                                flight2.toString());
                                if(this.TranList.size()>0){
                                    this.TranList.remove(0);
                                    if(this.TranList.size()>0)
                                        temp2=(temprecodtran)this.TranList.get(0);
                                }
                                transize--;
                                NoOfChoice++;
                             }

                     }

                     else{
                         this.jTextArea.append("\nNot Match\n");
                     }
                 
                 }
                 this.jTextArea.append("\n\n\n//////////////////////////////////Search Over//////////////////////////////////\n\n");
                 this.jTextArea.setEditable(false);
                 
             }catch(SQLException e){
                 //this.jTextArea.append(e.toString());
                 return;
             }
             
         }
     
}
class temprecoddirect{
    int id;
    long duration;
    float price;
    
    public temprecoddirect(int a,long b, float c){
        this.id=a;
        this.duration=b;
        this.price=c;
    }
    
}

class temprecodtran{
    int id1;
    int id2;
    long duration;
    float price;
    public temprecodtran(int a,int b,long d, float p){
        this.id1=a;
        this.id2=b;
        this.duration=d;
        this.price=p;
    }
}
 
class Searchinfor
 {
     int Infno; // 1 invalid input 2 no flight found 3 found success
     String message;
     int NoofItinerary;
     ItineraryInfo Itinfor=new ItineraryInfo();
     Flight flight=new Flight();
     //Searchinfor next=new Searchinfor();
     public Searchinfor(){
         Infno=-1;
         NoofItinerary=-1;
         message="ADSF";
     }
     
 }
 
class ItineraryInfo
{
    String day;
    String departtime;
    String Arrivaltime;
    int Fee;
    int duration;
    int NOofflight;
    public ItineraryInfo(){
        day="NO";
        departtime="NO";
        Arrivaltime="NO";
        Fee=0;
        duration=0;   
    }
}
/*
class Flight
{
    int FlightNo;
    String Airline;
    String Departcity;
    String Arrivalcity;
    String Departtime;
    String ArrivalTime;
    int price;
    int Noseet;
    //Flight next=new Flight();
    public Flight(){
        FlightNo=-1;
        price=-1;
        Noseet=-1;
        Airline="UDF";
        Departcity="UDF";
        Departtime="UDF";
        Arrivalcity="UDF";
        ArrivalTime="UDF";        
    }
}
*/


class CreateBooking
{
    String bookname;
    String PhoneNo;
    String BookCard;
    
    public CreateBooking() {
        bookname="UDF";
        PhoneNo="UDF";
        BookCard="UDF";
    }
    public String CreateBook(Search search){
        //Cratering booking process 
        //return the created booking id
        return "0000000001";
    }
}

class iusse
{
    String BookingID;
    String FlightNo;
    String FlightNo2;
    public iusse(){
        BookingID="UDF";
        FlightNo="UDF";
        FlightNo2="UDF";
    }
  //  public issueinfo issueprocess(){
  //      //process of issue a ticket
  //  }
    
}

class issueinfo
{
    String noofTicket;
    String bookingID;
    String FlightNO;
    String FlightNO2;
    String Passengername;
    
}

class admit
{
    public admit(){
        
    }
    public String deleteprocess(){
        //after process return information
        
        return "\nDelete all the tables.....Success\n";
    }
    
    public String Createprocess(){
        //after process return informations
        return "\nAll the tables are created\n";
    }
}

class confirm {
    public confirm(){
        
    }
    
    public String showprocess(){
        //need process
        return "\nAll the table in the database: \n......\n";
    }
    
    public String totalsal(){
        //need process
        return "\ntotal sale is\n......\n";
    }
    
    public String popular(int no){
        //need process
        return ("the"+Integer.toBinaryString(no)+ "most popular cities are\n......\n");
    }
       
}
         

         
    
      
         
         