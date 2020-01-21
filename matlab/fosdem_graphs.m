
close all; 
clear all;

M = 4; 
msg = randi([0 M-1],2500,1); % Random message
hMod = comm.QPSKModulator();
modmsg = hMod(msg); % Modulate using QPSK

modmsg = modmsg + 0.08*(randn(size(modmsg))+1j*randn(size(modmsg)));
figure; plot(real(modmsg)); hold on; plot(imag(modmsg), 'r');
xlabel('Sample Number');
ylabel('Voltage');
title('Transmitted Signal');
set(gca,'XLim',[0,100])
figure; plot(real(modmsg),imag(modmsg),'+', 'LineWidth',3, 'MarkerSize',10);
title('Line of Sight Only Constellation');

% 
chan = [1.0, 0 0 0 .3-.15j, 0 .2+.33j, 0 0 0 0 0 .1+.03j ]; % Channel coefficients
filtmsg = filter(chan,1,modmsg); % Introduce channel distortion
filtmsg = filtmsg + 0.1*(randn(size(filtmsg))+1j*randn(size(filtmsg)));
figure; plot(real(filtmsg)); hold on; plot(imag(filtmsg), 'r');
xlabel('Sample Number');
ylabel('Voltage');
title('Received Signal');
set(gca,'XLim',[0,100])
figure; plot(real(filtmsg),imag(filtmsg),'x', 'LineWidth',3, 'MarkerSize',10);
title('Constellation Received through Multipath Channel');






% filtmsg = resample(filtmsg,4,1);