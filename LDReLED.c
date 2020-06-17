#include <ADuC842.H>
#include <stdio.h>

float CONVERTION = 0.00061035;
//--------- Variáveis ------------------
//Duty
unsigned int Duty = 0;
float Duty_Cycle = 0;

//Temperatura
float temperatura = 0;

//Luminosidade
float luminosidade = 0;
float valor_ad_conv = 0;
float valor_ad = 0;

unsigned int bufferad = 0;
unsigned int temp = 0;
unsigned int i = 0;

//--------- Funções ------------------
//Sensores 
void ler_temperatura(void);
void ler_luminosidade(void);

//Serial
void configura_serial(void);
void envia_dados_serial(void);

//PWM
void configura_PWM(void);
void controle_PWM(void);

//Atuadores
void ativa_led_flag(void);
void ativa_ventoinha(void);

//Timer
void configura_timer(void);
					   

void main(void){
//	P3_4 = 1;
	PLLCON = 0x80;
	ADCCON1 = 0x80;
	
	configura_serial();
	configura_PWM();
	configura_timer();

	//P3_0 = 0;
	//P3_1 = 0;

	while(1){		
		ler_luminosidade();
		ler_temperatura();
		controle_PWM();
		ativa_led_flag(); // Atuador
	}
}
//--------- Configurações Iniciais ------------------
void configura_serial(void){
	T3CON = 0x85;
	T3FD = 0x2d;
	SCON = 0x52;
}
void configura_PWM(void){
	PWMCON = 0x93;
	PWM1H = 0xFF;
	PWM1L = 0xFF;
}
void configura_timer(void){
	TIMECON = 0x13;
	INTVAL = 0x01;	// 1 segundo

	IEIP2 = 0x44;
	EA = 1;
}
//--------- Funções Serial - Timer ------------------
void TIC_int () interrupt 10{
	printf("{\"Luz\" : %f, \"Temperatura\" : %f}\n",luminosidade, temperatura);
}
//--------- Funções dos Sensores ------------------
void ler_temperatura(){
  for(i = 0; i<16; i++){
		ADCCON2 = (ADCCON2 & 0xF0) | 0x07;
		SCONV = 1;

		while(SCONV)
			bufferad = ADCDATAL | ((ADCDATAH & 0x0F) << 8);
		temp = temp + bufferad;		
	}
	valor_ad = (temp/16);
	temp = 0;
	temperatura = valor_ad * CONVERTION;
}
void ler_luminosidade(){
	for(i = 0; i<16; i++){
		ADCCON2 = (ADCCON2 & 0xF0) | 0x03;
		SCONV = 1;

		while(SCONV)
			bufferad = ADCDATAL | ((ADCDATAH & 0x0F) << 8);
		temp = temp + bufferad;		
	}
	valor_ad = (temp/16);
	temp = 0;
	luminosidade = valor_ad * CONVERTION;
}

//--------- Funções de Controle ------------------
void controle_PWM(){
 	Duty = (valor_ad * 65535)/4095; 
 	PWM0H = Duty >> 8;
	PWM0L = (Duty & 0x00FF);
	Duty_Cycle = (Duty*100.0)/65535.0;
}

void ativa_led_flag(){
	if(valor_ad_conv < 1){
	  P3_4 = 0;
	}
	else{
	  P3_4 = 1;
	}
}
