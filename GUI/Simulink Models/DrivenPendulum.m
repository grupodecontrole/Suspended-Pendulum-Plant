%%this file contains the test parameters of the propeller pendulum
%%Developer: Yago Luiz Monteiro Silva

%parâmetros físicos 
Km = 0.0296; % rad/V
d  = 0.03; %m
m  = 0.36; %%quilogramas
c  = 0.0076;%N*m*s
J  = 0.0106;
g  = 9.81; %m/s²

%montagem da função de transferência 
num1 = [0 0 Km/J];
den1 = [ 1 c/J (m*g*d)/J];
pendulo = tf(num1,den1);

