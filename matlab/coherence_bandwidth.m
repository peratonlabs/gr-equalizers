close all; clear all;

rng(1441352253);
NFFT = 8192;
samp_rate = 1e6;
sym_rate = 1e3;

% coherence bandwidth smaller than the signal
B = 10*sym_rate;
D = 1/B
D_samps = floor(D*samp_rate)

h = randn(1,D_samps)+1j*randn(1,D_samps);
h = h / norm(h);
[H,W] = freqz(h,1,'whole',NFFT);
H = fftshift(H);
f = samp_rate*[(-NFFT/2):(NFFT/2-1)]/NFFT;
f = f/1000;

figure; plot(f,10*log10(abs(H)));
xlabel('Frequency (kHz)');
ylabel('Filter Magnitude Response (dB)');
xticks([-500:100:500]);

title(sprintf('Coherence BW of %d kHz',B/1000));

% coherence bandwidth larger than the signal
sym_rate = 20e3;

B = 10*sym_rate;
D = 1/B
D_samps = D*samp_rate

h = randn(1,D_samps)+1j*randn(1,D_samps);
h = h / norm(h);
[H,W] = freqz(h,1,'whole',NFFT);
H = fftshift(H);
f = samp_rate*[(-NFFT/2):(NFFT/2-1)]/NFFT;
f = f/1000;

figure; plot(f,10*log10(abs(H)));
xlabel('Frequency (kHz)');
ylabel('Filter Magnitude Response (dB)');
xticks([-500:100:500]);

title(sprintf('Coherence BW of %d kHz',B/1000));


