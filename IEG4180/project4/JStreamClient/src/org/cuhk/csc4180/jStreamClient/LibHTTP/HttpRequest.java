/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cuhk.csc4180.jStreamClient.LibHTTP;


import java.util.*;
import java.util.regex.*;

/**
 *
 * @author hguan5
 */
public class HttpRequest {
    private String m_method;
    private String m_path;
    private String m_version;
    private HttpHeader m_header;
    private byte[] m_message;
    
    
    public HttpRequest(){
        m_method = null;
        m_path = null;
        m_version = null;
        m_header = new HttpHeader();
        m_message = null;
    }
    
    public void parse(String str) throws Exception{
        parse(str.getBytes());
    }
    
    public void parse(byte[] bytes) throws Exception{
        String str = new String(bytes);
        Pattern pattern = null;
        Matcher matcher = null;
        boolean found = false;
        int firstEnd = 0;
        int lastEnd = 0;
        
        pattern = Pattern.compile("(?m)^(HEAD|GET|POST|PUT|DELETE|TRACE|OPTIONS|CONNECT) (.*) HTTP/(\\d.\\d)$");
        matcher = pattern.matcher(str);
        found = matcher.find();
        if(found){
            m_method = matcher.group(1);
            m_path = matcher.group(2);
            m_version = matcher.group(3);
        }
        else
            throw new Exception();
        firstEnd = matcher.end();
        
        pattern = Pattern.compile("(?m)^(.+): ?(.+)$");
        matcher = pattern.matcher(str.substring(firstEnd)); 
        while(matcher.find()){
            m_header.add(matcher.group(1), matcher.group(2));
            lastEnd = matcher.end();
        }
        
        if(lastEnd > 0){
            m_message = new byte[bytes.length - 2 - firstEnd - lastEnd];
            for(int i=0;i<m_message.length;i++){
                m_message[i] = bytes[i + 2 + firstEnd + lastEnd];
            }
        }
        
    }
    
    public String getPath(){
        return m_path;
    }
    
    
    @Override
    public String toString(){
        String str = new String();
        str += m_method + " " + m_path + " ";
        str += "HTTP/"+ m_version + "\n";
        str += m_header.toString();
        
        return str;
    }
}
