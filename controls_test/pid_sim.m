%% Jay PID Simulation
% Sargis S Yonan

k_p = 2.8;
k_d = 1.6;
k_i = 0.3;

s = tf('s');

K = k_p + (k_i / s) + (k_d * s);

fprintf('Closed loop PID\n')
feedback(K, 1)


figure()
rlocus(feedback(K, 1))
figure()
plot(step(feedback(K, 1)))