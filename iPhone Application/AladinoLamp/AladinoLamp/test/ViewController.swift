//
//  ViewController.swift
//  Aladinos Lamp
//
//  Created by Enmanuel Gonçalves on 04/10/17.
//  Copyright © 2017 Enmanuel Gonçalves. All rights reserved.
//

import UIKit
import SwiftSocket

class ViewController: UIViewController, UITextFieldDelegate {
    
    @IBOutlet weak var ipaddress: UITextField!
    @IBOutlet weak var label: UILabel!
    @IBOutlet weak var menu: UIView!
    @IBOutlet weak var displayColor: UILabel!
    @IBOutlet weak var rSlider: UISlider!
    @IBOutlet weak var gSlider: UISlider!
    @IBOutlet weak var bSlider: UISlider!
    @IBOutlet weak var intensity: UISlider!
    @IBOutlet weak var tempLabel: UILabel!
    @IBOutlet weak var graus: UILabel!
    
    //Inicialização de variaveis
    var ipadd = String()
    var cliente: TCPClient!
    var timer = Timer()

    //Função executada quando carrega a view
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        self.ipaddress.delegate = self;
    }

    // Função executada quando se aperta o botão connect
    @IBAction func connect(_ sender: Any) {
        ipadd = ipaddress.text!
        cliente = TCPClient(address: ipadd, port: 10101)
        switch cliente.connect(timeout: 10){
        case .success:
            label.text = "Connected"
            menu.isHidden = false
            temporizador()
        case .failure(let error):
            print(error)
            label.text = "Error na ligação"
        }
        
    }
 
    // Envia o modo rainbow ao presionar o botão
    @IBAction func rainbowbtn(_ sender: Any) {
        setMode(tipo: 2, mode: 2)
    }
    
    // Envia o modo temperatura ao presionar o botão
    @IBAction func Temp_Mode(_ sender: Any) {
        setMode(tipo: 2, mode: 1)
        requestTemp()
    }
    
    // Envia a cor quando é alterado o slider da cor vermelha
    @IBAction func redChange(_ sender: Any) {
        changeColorLB(r: CGFloat(rSlider.value), g: CGFloat(gSlider.value), b: CGFloat(bSlider.value))
    }
    
    // Envia a cor quando é alterado o alider da cor verde
    @IBAction func greenChange(_ sender: Any) {
        changeColorLB(r: CGFloat(rSlider.value), g: CGFloat(gSlider.value), b: CGFloat(bSlider.value))
    }
    
    // Envia a cor quando é alterado o slider azul
    @IBAction func blueChange(_ sender: Any) {
        changeColorLB(r: CGFloat(rSlider.value), g: CGFloat(gSlider.value), b: CGFloat(bSlider.value))
    }
    
    // Envia a intensidade e cor quando o slider da intensidade é alterado
    @IBAction func intensityChange(_ sender: Any) {
        changeColorLB(r: CGFloat(rSlider.value), g: CGFloat(gSlider.value), b: CGFloat(bSlider.value))
        let inten = Int(intensity.value)
        displayColor.text = String(inten)
    }
    
    
    // Função para executar a conecção a lampada se esta desconectada
    func connection(){
        switch cliente.connect(timeout: 10){
        case .success:
            label.text = "Tente Novamente"
        case .failure(let error):
            print(error)
            label.text = "Falha no envio"
        }
    }
    
    // Função para enviar a cor para a lampada
    func changeColorLB(r: CGFloat, g: CGFloat, b: CGFloat) {
        //displayColor.backgroundColor = UIColor(red: r/255, green: g/255, blue: b/255, alpha: 1)
        
        var dados = [UInt8]()
        dados.append(UInt8(3))
        dados.append(UInt8(Int(rSlider.value)))
        dados.append(UInt8(Int(gSlider.value)))
        dados.append(UInt8(Int(bSlider.value)))
        dados.append(UInt8(Int(intensity.value)))
        switch cliente.send(data: dados){
        case .success:
            displayColor.backgroundColor = UIColor(red: r/255, green: g/255, blue: b/255, alpha: 1)
            //displayColor.text = "enviado"
        case .failure(let error):
            print(error)
            connection()
    }
}
    
    // Função para enviar o modo
    func setMode(tipo:Int, mode: Int){
        
        var dados = [UInt8]()
        dados.append(UInt8(tipo))
        dados.append(UInt8(mode))
        switch cliente.send(data: dados){
        case .success:
            if (mode == 2){
                displayColor.text = "Rainbow Mode"
            } else if (mode == 1){
                displayColor.text = "Temp_Mode"
            }
        case .failure(let error):
            print(error)
            connection()
        }
    }
    
    // Função que pede a temperatura e a mostra no display
    @objc func requestTemp(){
        var dados = [UInt8]()
        dados.append(UInt8(6))
        switch cliente.send(data: dados){
        case .success:
            guard let data = cliente.read(1024*10) else {return}
            if (data.count >= 3){
                print("recebeu")
                graus.text = "ºC"
                tempLabel.text = String(data[2])
            }
            print(data)
        case .failure(let error):
            print(error)
            connection()
        }
    }
    
    // Temporizador de segundo a segundo
    func temporizador(){
        timer = Timer.scheduledTimer(timeInterval: 4, target: self, selector: #selector(ViewController.requestTemp), userInfo: nil, repeats: true)
    }
    
    // função para poder fechar o teclado ao inserir o endereço IP
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        self.view.endEditing(true)
        return false
    }

}
