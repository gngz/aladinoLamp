import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.PrintStream;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Scanner;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.*;

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
    public final byte SETDATE = 4;
    public final byte SETTIME = 5;
    public final byte SETCOLOR = 3;
    public final byte SETMODE = 2;
    public final byte  ACK = 0; //echo
    public final byte setMode = 3;
    public final byte  requestTemp = 6;
    public final byte  sendTemp = 7;
    private Socket cliente = null;
    private DataInputStream din;
    private DataOutputStream dout;
    public TempThread temp = null; //meter private
    private String Address;
    private int Port;
    JFrame j ;
    
    
    public Cliente(String Address, int Port, boolean tipoConnect, JFrame jf){ //adicionar porta
        if(tipoConnect){
            this.Address = Address;
            this.Port = Port;
            this.j = jf;
            Initialize(tipoConnect);
            
        }
        
        
    }
        class TempThread implements Runnable { // Thread temp

        Thread t;
        DataInputStream din = null;
        DataOutputStream dout = null;       
        int [] tempe = new int [3];
        boolean parar = false;
        
        public TempThread(DataInputStream din, DataOutputStream dout) 
        {
             t = new Thread(this);
             t.start();
             this.din = din;
             this.dout = dout;
             
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
                            
                          ((NewJFrame) j).jLabel5.setText(temperatura);
                        }
                    }
                    ) ;
        }
        

        @Override
        public void run(){
            
            while(true){
                try {
                    Thread.sleep(1000); // requests de 1 a 1 secs
                } catch (InterruptedException ex) {
                    Logger.getLogger(Cliente.class.getName()).log(Level.SEVERE, null, ex);
                }
                try {
                    sendData(requestTemp);
                    
                    din.read();
                    
                    tempe[0] = din.read();
                    tempe[1] = din.read();
                    tempe[2] = din.read();
                    
                    
                    
                    float temper = tempe[1] + ((float) tempe[2]/100);
                    if(tempe[0] == 1)
                        temper = temper * -1;
                    
                    
                    System.out.printf("Temperatura: %.2f ºC \n",temper);
                    
                    setTempLabel(temper);
                    
                } catch (IOException ex) {
                    System.out.println("Erro no envio de pedido de temperatura");
                   
                }
                
                 System.out.println("Pedido de temperatura enviado");
                 
                 while(parar){
                    try {
                        synchronized(this){
                            System.out.println("Thread suspendido");
                            wait();
                        }
                    } catch (InterruptedException ex) {
                        Logger.getLogger(Cliente.class.getName()).log(Level.SEVERE, null, ex);
                    }
                 }
                 
            }
        }     
    }
                      
    public void Initialize(boolean tipoConnect){ // para os connects e disconnects sucessivos (se houver)
        try {
            if(!tipoConnect){  
                     
                if(!findAddr()){
                    System.out.println("Nao encontrado");
                    return;
                }
                
                } 
            cliente = new Socket();
            cliente.connect(new InetSocketAddress(Address, Port), 100); // sobrepoem o endereço do construtor pelo address do findAddr
            
        } catch (IOException ex) {
                    System.out.println("Endereço nao existente");
                    return;
        }
        System.out.println("Utilizador ligado com sucesso no endereço "+cliente.getInetAddress());
        try {
            dout =  new DataOutputStream(cliente.getOutputStream());
            din = new DataInputStream(cliente.getInputStream());
        } catch (IOException ex) {
           System.out.println("Erro ao criar outputStream");
        }
        temp = new TempThread(din,dout); //inicia a thread
        
    }
    
    public boolean getisConnected(){
        System.out.println(cliente.isConnected());
        return true;
        //return cliente.is
    }  
    
   public void killCurrentServ()
    {
        dout = null;
        din = null;
        try {
            cliente.close();
        } catch (IOException ex) {
           System.out.println ("Cliente socket fechado"); //quando pressionar o botão disconnect.
        }             
    }
    
   public void setParar(boolean parar){
       this.temp.parar = parar;
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
    }
    
    public void sendData(byte tipo) throws IOException{ // exemplos: echos, request_temp, etc
   
        dout.write(tipo);
       
 }
    
    public void receiveData(){
        System.out.println("Teste");
    }
    
    public boolean findAddr() throws UnknownHostException, IOException{ // ALPHA MUITO ALPHA
        InetAddress localhost = InetAddress.getLocalHost();

        byte[] ip = localhost.getAddress();
        for (int i = 1; i <= 254; i++) //redes 255.255.255.0 /24
        {
            ip[3] = (byte)i;
            InetAddress address = InetAddress.getByAddress(ip);
            if (address.isReachable(1000))
            {
                System.out.println("ENCONTROU O ENDEREÇO: "+address.getHostAddress());
                Address = address.getHostAddress();
                return true;
            }
            
        }
        return false;
    }
}
 
