clear all; close all;
len_response = 24;
decay = 0.4;

rng(1443583472);

h = randn(1,len_response)+1j*randn(1,len_response);

h = h.*exp(-decay*([0:(len_response-1)]));

h = h./norm(h);

% freqz(h);

fprintf("%f+%fj,",real(h),imag(h));