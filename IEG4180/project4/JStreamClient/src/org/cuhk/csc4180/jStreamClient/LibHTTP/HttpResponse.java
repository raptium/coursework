/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cuhk.csc4180.jStreamClient.LibHTTP;

import java.text.SimpleDateFormat;
import java.util.*;
import java.util.regex.*;

/**
 *
 * @author hguan5
 */
public class HttpResponse {
    private String m_status;
    private String m_version;
    private HttpHeader m_header;
    private byte[] m_message;
    private Hashtable<String, String> m_statusCode;
    
    
    public HttpResponse(){
        Date date = new Date();
        SimpleDateFormat format = new SimpleDateFormat("EEE, dd MMM yyyy HH:mm:ss z");
        
        m_status = null;
        m_version = "1.1";
        m_header = new HttpHeader();
        m_header.add("Cache-Control", "no-cache");
        m_header.add("Connection", "close");
        m_header.add("Date", format.format(date));
        m_header.add("Content-Type", "text/html; charset=us-ascii");
        m_header.add("Accept-Ranges", "bytes");
        
        putStatusCode();
    }
    
    
    private void putStatusCode(){
        m_statusCode = new Hashtable<String, String>();
        m_statusCode.put("100", "Continue");
        m_statusCode.put("101", "Switching Protocols");
        m_statusCode.put("200", "OK");
        m_statusCode.put("201", "Created");
        m_statusCode.put("202", "Accepted");
        m_statusCode.put("203", "Non-Authoritative Information");
        m_statusCode.put("204", "No Content");
        m_statusCode.put("205", "Reset Content");
        m_statusCode.put("206", "Partial Content");
        m_statusCode.put("300", "Multiple Choices");
        m_statusCode.put("301", "Moved Permanently");
        m_statusCode.put("302", "Found");
        m_statusCode.put("303", "See Other");
        m_statusCode.put("304", "Not Modified");
        m_statusCode.put("305", "Use Proxy");
        m_statusCode.put("307", "Temporary Redirect");
        m_statusCode.put("400", "Bad Request");
        m_statusCode.put("401", "Unauthorized");
        m_statusCode.put("402", "Payment Required");
        m_statusCode.put("403", "Forbidden");
        m_statusCode.put("404", "Not Found");
        m_statusCode.put("405", "Method Not Allowed");
        m_statusCode.put("406", "Not Acceptable");
        m_statusCode.put("407", "Proxy Authentication Required");
        m_statusCode.put("408", "Request Time-out");
        m_statusCode.put("409", "Conflict");
        m_statusCode.put("410", "Gone");
        m_statusCode.put("411", "Length Required");
        m_statusCode.put("412", "Precondition Failed");
        m_statusCode.put("413", "Request Entity Too Large");
        m_statusCode.put("414", "Request-URI Too Large");
        m_statusCode.put("415", "Unsupported Media Type");
        m_statusCode.put("416", "Requested range not satisfiable");
        m_statusCode.put("417", "Expectation Failed");
        m_statusCode.put("500", "Internal Server Error");
        m_statusCode.put("501", "Not Implemented");
        m_statusCode.put("502", "Bad Gateway");
        m_statusCode.put("503", "Service Unavailable");
        m_statusCode.put("504", "Gateway Time-out");
        m_statusCode.put("505", "HTTP Version not supported");
    }
    
    public void setStatus(String status) throws Exception{
        Pattern pattern = Pattern.compile("^\\d{3}$");
        Matcher matcher = pattern.matcher(status);
        boolean found = matcher.find();
        
        if(found){
            m_status = status;
        }else
            throw new Exception("Known Http Status");
    }
    
    public void fillMessage(byte[] message){
        m_message = message;
        m_header.add("Content-Length", "" + m_message.length);
    }
    
    public byte[] toBytes(){
        String str = new String();
        str += "HTTP/" + m_version + " ";
        str += m_status + " " + (String)m_statusCode.get(m_status) + "\n";
        str += m_header.toString();
        str += "\n";
        
        byte[] buffer = new byte[str.length() + m_message.length];
        System.arraycopy(str.getBytes(), 0, buffer, 0, str.length());
        System.arraycopy(m_message, 0, buffer, str.length(), m_message.length);
        
        return buffer;
    }

}
