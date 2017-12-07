  import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.PrintStream;
import java.net.Socket;
import java.util.Scanner;
import java.util.logging.Level;
import java.util.logging.Logger;

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author André
 */
public class Cliente {

     //meter encapsulamento
    Socket cliente = null;
    static DataInputStream din;
    static DataOutputStream dout;
    TempThread temp = null;
    String Address;
    int Port;
    
    public Cliente(String Address, int Port){ //adicionar porta
        this.Address = Address;
        this.Port = Port;
        
    }
        class TempThread implements Runnable { // Thread temp

        Thread t;
        DataInputStream din = null;
        DataOutputStream dout = null;       

        public TempThread(DataInputStream din, DataOutputStream dout) 
        {
             t = new Thread(this);
             t.start();
             this.din = din;
             this.dout = dout;
        }

        @Override
        public void run(){
            
            try {
                PrintStream saida;
                
                saida = new PrintStream(cliente.getOutputStream());
                
                
                Scanner teclado = new Scanner(System.in);
                while (teclado.hasNextLine()) {
                    saida.println(teclado.nextLine());
                }
            } catch (IOException ex) {
                Logger.getLogger(Cliente.class.getName()).log(Level.SEVERE, null, ex);
            }
        }     
      }
                      
    public void Initialize(){ // para os connects e disconnects sucessivos (se houver)
        try {
            cliente = new Socket(Address,8080);
            System.out.println(cliente.isConnected());
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
        temp = new TempThread(din,dout);
        
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
    
    
    
    protected void sendData(byte [] tipo, int size ,int[] valores) throws IOException 
    {     
    byte [] data = new byte [size]; //size
    data[0] = tipo[0];;
    for(int i = 1; i < data.length; i++){
        data[i] = (byte) valores[i-1]; // valores rbg por exemplo. Array de ints SIGNED
        }
            
            dout.write(data, 0, data.length); // a testar com o node mcu
            
            for(int i = 0; i < data.length; i++){
                System.out.print(data[i]+" ,");
            }
    }
    
    public void receiveData(){
        
    }
    
}
 
