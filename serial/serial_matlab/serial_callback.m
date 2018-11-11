function [ ] = serial_callback(s1)
% Serial Test Callback function

while(s1.BytesAvailable > 0)
    data = fgetl(s1);    % Read line of text from serial and discard terminator
    %data = fgets(s1);    % Read line of text from serial and include terminator
    %data = fscanf(s1);   % Read data from instrument, and format as text
	display(data);
end

end

