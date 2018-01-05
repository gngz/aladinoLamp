import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintStream;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.net.UnknownHostException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.Scanner;
import java.util.concurrent.TimeoutException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.*;
import java.net.SocketTimeoutException;
import java.util.Calendar;
import org.jfree.data.category.DefaultCategoryDataset;
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author André Moura
 */
public class Cliente {

     //meter encapsulamento
    private final byte TEMP = 1;
    private final byte SETDATE = 4;
    private final byte SETTIME = 5;
    private final byte SETCOLOR = 3;
    private final byte SETMODE = 2;
    private final byte  ACK = 0; //echo
    private final byte  requestTemp = 6;
    private final byte  sendTemp = 7;
    private Socket cliente = null;
    private DataInputStream din;
    private DataOutputStream dout;
    private TempThread temp = null; //meter private
    private String Address;
    private int Port;
    private Janela frame;
    final int PORT = 80;//6442
    private boolean is_Connected = false;
    private Calendar now = Calendar.getInstance();
    
    public Cliente(String Address, int Port, boolean tipoConnect, Janela frame) throws IOException { //adicionar porta
        if(tipoConnect){
            this.Address = Address;
        }
            this.Port = PORT;
            this.frame = frame;       
            
            if(!Initialize(tipoConnect))
            {
                cliente = null;
            }  
    }
    public Socket getCliente() {
        return cliente;
    }
    
     public byte getSETCOLOR() {
        return SETCOLOR;
    }

    public TempThread getTemp() {
        return temp;
    }
        
    /**
     * @return the SETMODE
     */
    public byte getSETMODE() {
        return SETMODE;
    }

    /**
     * @return the requestTemp
     */
    public byte getRequestTemp() {
        return requestTemp;
    }
    
        class TempThread implements Runnable { // Thread temp
            
        private Thread t;
        private DataInputStream din = null;
        private DataOutputStream dout = null;       
        private int [] tempe = new int [3];
        private boolean parar = false;
        private ArrayList<Float> temperaturas;
        private File arquivo = null;
        private FileWriter fw = null; //para a escrita no ficheiro
        private BufferedWriter bw = null;
        private DefaultCategoryDataset dataSet =  new DefaultCategoryDataset();   
        private double i = 1;
        
        public TempThread(DataInputStream din, DataOutputStream dout) throws IOException 
        {
             t = new Thread(this);
             t.start();
             this.din = din;
             this.dout = dout;
             temperaturas = new ArrayList<Float>();
             initializaFile("temperatura");
        }
        
     private DefaultCategoryDataset createDataset(double temperatura) {
      
      dataSet.addValue(temperatura, "", ""+(int)i);
      
       return dataSet;
  }  
        
        private void initializaFile(String nome) throws IOException{
            
            arquivo = new File("Registo de "+nome+".csv");
            try {
                FileWriter fw = new FileWriter(arquivo);
            } catch (IOException ex) {
                System.out.println("Erro ao abrir o ficheiro");
                now = Calendar.getInstance();
                frame.logsList.add("["+now.get(Calendar.HOUR_OF_DAY) + ":" + now.get(Calendar.MINUTE)+"]"+" "+ "Erro ao abrir o ficheiro \n");
            }
            FileWriter fw = new FileWriter(arquivo);
            bw = new BufferedWriter(fw);
            bw.write("\"Tempo\""+";"+"\"Temperatura\"");
            bw.newLine();
        }
        
        public void escreveFich() throws IOException
        {
            int tamanho = temperaturas.size();
                
            DateFormat dataFormat = new SimpleDateFormat ("yyyy/MM/dd HH:mm:ss");
            Date date = new Date();
 
            bw.write("\""+dataFormat.format(date)+"\""+";"+"\"" +temperaturas.get(tamanho - 1).floatValue()+" ºC"+"\"");
            System.out.println(temperaturas.get(tamanho - 1).floatValue());
            
            bw.newLine();
            bw.flush(); // para atualizar o ficheiro
          
        }

        synchronized void recomeçar() {
            this.parar = false;
            notify(); 
        }
        
        public void setTempLabel(float temper)
        {
            SwingUtilities.invokeLater(new Runnable()
                    {
                        public void run()
                        {
                            String temperatura = new String();
                            
                            temperatura = String.format("%.2f ºC", temper);
                            frame.getlabelTemp().setText(temperatura);
                        }
                    }
                    ) ;
        }
        

        @Override
        public void run(){
            
            while(!t.isInterrupted()){
                try {
                    Thread.sleep(1000); // requests de 1 a 1 secs
                } catch (InterruptedException ex) {
                   System.out.println("Thread Halt");
                   now = Calendar.getInstance();
                   frame.logsList.add("["+now.get(Calendar.HOUR_OF_DAY) + ":" + now.get(Calendar.MINUTE)+"]"+" "+ "Thread Halt\n");
                   return;
                }
                try {
                    sendData(getRequestTemp());
                    System.out.println("Parar ="+parar);
                    din.read();
                    
                    tempe[0] = din.read();
                    tempe[1] = din.read();
                    tempe[2] = din.read();
                    
                    float temper = tempe[1] + ((float) tempe[2]/100);
                    double temper2 = tempe[1] + ((double) tempe[2]/100);
                    if(tempe[0] == 1)
                        temper = temper * -1;
                    
                    i = i+ 0.5;
                    System.out.printf("Temperatura: %.2f ºC \n",temper);
                    now = Calendar.getInstance();
                    frame.logsList.add("["+now.get(Calendar.HOUR_OF_DAY) + ":" + now.get(Calendar.MINUTE)+"]"+" "+ "Temperatura: \n"+temper+"ºC");
                    temperaturas.add(new Float(temper));
                    frame.setValoresGraph(createDataset(new Double(temper2)));
                 
                    escreveFich();
                    setTempLabel(temper);
                    
                    
                } catch (IOException ex) {
                    System.out.println("Erro no envio de pedido de temperatura");
                    now = Calendar.getInstance();
                    frame.logsList.add("["+now.get(Calendar.HOUR_OF_DAY) + ":" + now.get(Calendar.MINUTE)+"]"+" "+ "Erro no envio de pedido de temperatura\n");
                   
                }
                
                 System.out.println("Pedido de temperatura enviado");
                 now = Calendar.getInstance();
                 frame.logsList.add("["+now.get(Calendar.HOUR_OF_DAY) + ":" + now.get(Calendar.MINUTE)+"]"+" "+ "Pedido de temperatura enviado\n");
                 System.out.println("Parar ="+parar);
                 while(parar){
                    try {
                        synchronized(this){
                            System.out.println("Chegou aki");
                            System.out.println("Thread suspendido");
                            now = Calendar.getInstance();
                            frame.logsList.add("["+now.get(Calendar.HOUR_OF_DAY) + ":" + now.get(Calendar.MINUTE)+"]"+" "+ "Thread suspendido\n");
                            wait();
                        }
                    } catch (InterruptedException ex) {
                        System.out.println("Thread susp");
                    }
                 }
                 
            }
        }     
    }
                      
    public boolean Initialize(boolean tipoConnect) throws IOException { // para os connects e disconnects sucessivos (se houver)
                 
            if(!tipoConnect){  
                     
                if(!findAddr()){
                    System.out.println("Nao encontrado");
                    now = Calendar.getInstance();
                    frame.logsList.add("["+now.get(Calendar.HOUR_OF_DAY) + ":" + now.get(Calendar.MINUTE)+"]"+" "+ "Search automatico nao encontrou host\n");
                    setbarraProgresso(0);
                    frame.setcancelarProgresso(false);
                    frame.getjanelaProgresso().setVisible(false);
                    return false;
                }
                
            }
            else {
               try{
                   
                cliente = new Socket();
                cliente.connect(new InetSocketAddress(Address, Port), 100); // sobrepoem o endereço do construtor pelo address do findAddr
                
               }
               catch (SocketTimeoutException e) {
                   System.out.println("Erro de Timeout");
                   now = Calendar.getInstance();
                   frame.logsList.add("["+now.get(Calendar.HOUR_OF_DAY) + ":" + now.get(Calendar.MINUTE)+"]"+" "+ "Erro de Timeout");
                    return false;
	        } 
               catch (IOException e) {
                  System.out.println("Input");
	         return false;
                }
               
            }    
       
        System.out.println("Utilizador ligado com sucesso no endereço "+cliente.getInetAddress());
        now = Calendar.getInstance();
        frame.logsList.add("["+now.get(Calendar.HOUR_OF_DAY) + ":" + now.get(Calendar.MINUTE)+"]"+" "+ "Utilizador ligado com sucesso no endereço "+cliente.getInetAddress());
        
        is_Connected = true;
        try {
            dout =  new DataOutputStream(cliente.getOutputStream());
            din = new DataInputStream(cliente.getInputStream());
        } catch (IOException ex) {
           System.out.println("Erro ao criar outputStream");
           now = Calendar.getInstance();
           frame.logsList.add("["+now.get(Calendar.HOUR_OF_DAY) + ":" + now.get(Calendar.MINUTE)+"]"+" "+ "Erro ao criar outputStream");
        }
        temp = new TempThread(din,dout); //inicia a thread
        
        return true;
    }
    
    public boolean getisConnected(){
        
        return is_Connected;
    }  
    
   public void killCurrentServ()
    {
        dout = null;
        din = null;
        temp.din = null;
        temp.dout = null;
        temp.t.interrupt();
        
        try {
            cliente.close();
        } catch (IOException ex) {
           System.out.println ("Erro ao fechar o socket"); //quando pressionar o botão disconnect.
           now = Calendar.getInstance();
           frame.logsList.add("["+now.get(Calendar.HOUR_OF_DAY) + ":" + now.get(Calendar.MINUTE)+"]"+" "+ "Erro ao fechar o socket");
        }
        System.out.println ("Cliente socket fechado"); //quando pressionar o botão disconnect.
        now = Calendar.getInstance();
        frame.logsList.add("["+now.get(Calendar.HOUR_OF_DAY) + ":" + now.get(Calendar.MINUTE)+"]"+" "+ "Cliente socket fechado");
    }
    
   public void setParar(boolean parar){
       this.getTemp().parar = parar;
   }
   
   synchronized void recomeçar() {
      this.temp.parar = false;
      notify(); 
   }
   
    
    
    public void sendData(byte tipo, int size ,int[] valores) throws IOException 
    {     
    byte [] data = new byte [size+1]; //size
    data[0] = tipo; // previous tipo[0]
    for(int i = 1; i < data.length; i++)
        {
        data[i] = (byte) valores[i-1]; // valores rbg por exemplo. Array de ints SIGNED
        }
            
            dout.write(data, 0, data.length); // a testar com o node mcu
            
            System.out.print("comando(");
            for(int i = 0; i < data.length; i++){
                
                
                if(i == data.length - 1)
                {
                     System.out.print(data[i]);
                }
                else
                {
                    System.out.print(data[i]+" ,");
                }
            }
            System.out.println(")");
            
    }
    
    public void sendData(byte tipo, int valor) throws IOException{ //para envio de valores unicos (sem array)
        
        byte [] data = new byte [2];
        data[0] = tipo;
        data[1] = (byte) valor;
        
        dout.write(data, 0, 2);
        System.out.println(data[0]);
        System.out.println();
    }
    
    public void sendData(byte tipo) throws IOException{ // exemplos: echos, request_temp, etc
   
        dout.write(tipo);
       
 }

    public boolean findAddr() throws UnknownHostException, IOException{ 
        InetAddress localhost = InetAddress.getLocalHost();
        
        byte[] ip = localhost.getAddress();
        
        for (int i = 1; i <= 254; i++) //redes 255.255.255.0 /24
        {
            if(frame.getcancelarProgresso()){
                return false;
            }
            setbarraProgresso(i); // atualiza o UI
            ip[3] = (byte)i;
            InetAddress address = InetAddress.getByAddress(ip);
            if (address.isReachable(10))
            {
                System.out.println("Encontrou o endereço na rede: "+address.getHostAddress());
                Address = address.getHostAddress();
                try
                {
                cliente = new Socket();
                cliente.connect(new InetSocketAddress(Address, Port), 10); // sobrepoem o endereço do construtor pelo address do findAddr
                is_Connected = true;
                return true;
                } 
                catch (IOException ex) {
                    System.out.println("Procurando...");
                    cliente.close();
                  }              
            }
        }
            return false;
    }
    
    public void setbarraProgresso(int valor){
             
    SwingUtilities.invokeLater(new Runnable()
                    {
                        public void run()
                        {
                            frame.getbarraProgresso().setValue(valor);
                        }
                    }
                    ) ;    
    }     
}