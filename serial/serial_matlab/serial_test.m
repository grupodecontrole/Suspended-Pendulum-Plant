%% INITIAL COMMANDS

% Clear workspace
clear;
close;
clc;

% find serial ports available
serial_list = instrfind;

% clear all serial ports
delete(serial_list);

% get serial ports hardware info
serial_info = instrhwinfo('serial');

% get serial availables
serial_availables = serial_info.AvailableSerialPorts;
disp(serial_availables);

%% SET SERIAL OBJECT

% get second serial available 
% (can change here. put the serial port name where arduino is connected)
serial_port_number = serial_availables{1};

% Create serial objetc
s1 = serial(serial_port_number);

% Set Serial General Configurations
set(s1, 'StopBit', 1);
set(s1, 'DataBits', 8);        
set(s1, 'BaudRate', 9600);
set(s1, 'Timeout',10);
set(s1, 'Terminator', 'CR/LF');     % CR = \r ; LF = \n ; 
                                    % CR/LF = \r\n ; LF/CR = \n\r 
                                    % EOL = end of line = CR/LF 'or' LF/CR
set(s1, 'FlowControl', 'none');     
set(s1, 'InputBufferSize', 128);    
s1.ReadAsyncMode = 'continuous';    

% Set Serial Callback Function Configuration
s1.BytesAvailableFcnMode = 'byte';  % read information as a byte
s1.BytesAvailableFcnCount = 5;      % how many bytes received to call function
s1.BytesAvailableFcn = ...
         @(~,~)serial_callback(s1); % s1 as an argument, and ignore two default
                                   	% callback functions arguments (~,~)

% Show Serial Configurations
get(s1);
disp(get(s1,'BaudRate'));

% Open Serial Communication
fopen(s1);

% Get identification number of serial device
% fprintf(s,'*IDN?');

%% SEND AND RECEIVE CHARACTERS

%Send character
while(true)
fwrite(s1,'H');
pause(10);
fprintf(s1,'L');

%Read Character

    if(s1.BytesAvailable > 0)
        data = fgetl(s1);    % Read line of text from serial and discard terminator
        %data = fgets(s1);    % Read line of text from serial and include terminator
        %data = fscanf(s1);   % Read data from instrument, and format as text
        display(data);
    end
end

%% FINISH COMMUNICATION

% Close Serial Communication
fclose(s1);

% Removes Serial From Memory
delete(s1);

% Clear Serial Object
clear s1;

%% END