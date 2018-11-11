%
% File:   InvertedPendulumPlant.m
% Author: Julio Cesar Ferreira Lima
% Project: Pendulum Inverted Plant
% This file was developed by help of this: https://www.youtube.com/watch?v=Vz8ESnfj_Jw
% 

% Clean all opened serial instruments.
delete(instrfindall); 
clear all;
close all;
clc;

warning('off','all')
warning


% Have to implement this, before.
%disp('In below the available serial ports:')
%seriallist
disp(instrhwinfo('serial'));

% Assigns the object s to serial port, and configs.
serialPort = ''; %input('Type the number of port that you wanna use: ');
s = serial(strcat('COM4' , int2str(serialPort)));             
set(s, 'InputBufferSize', 128);  
set(s, 'FlowControl', 'none');
set(s, 'BaudRate', 19200);
s.ReadAsyncMode = 'continuous';
set(s, 'DataBits', 8);        
set(s, 'StopBit', 1);
set(s, 'Timeout',5);       
% Show configs.
prop(1)=(get(s,'BaudRate'));                                 
prop(2)=(get(s,'DataBits'));
prop(3)=(get(s, 'StopBit'));
prop(4)=(get(s, 'InputBufferSize'));
disp(['Configurations: ',strcat((get(s,'Name'))),'  ',num2str(prop)]); 

% Opens the serial port.
fopen(s);                   

t=1;
received = '';

    T = 500;
    passo = 1;
    t=1;
    x=0;
while(true)
    
    received = strsplit(fgetl(s));

    number = str2double(received(5));
    %disp(number);
    
    
    x=[x,number];
    if(mod(numel(x),50) == 0)
            plot(x);
    end
    disp(mod(numel(x),10))

    %pay attention to this command %
    axis([T*fix(t/T),T+T*fix(t/T),0,1024]); 
    
    t=t+passo;
    drawnow;

end


