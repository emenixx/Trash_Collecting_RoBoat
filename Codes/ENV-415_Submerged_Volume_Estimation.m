% ce code permet de déterminer la ligne de flotaison à partir du poids du
% bateau


% caractéristiques environnementales
rho = 997; % kg/m^3

% caractéristiques du bateau
L = 1.055; % m
b = 0.095; % m
B = 0.16; % m
H = 0.2; % m
V_flot = (B+b)*H*L/2; % m^3, volume d'un seul flotteur
p = (B-b)/(H); % pente
m = [25,40]; % kg

% axe des x
M = 0:100; % kg

% axe des y : pourcentage immergé
P_imm = M.*100./(2.*V_flot.*rho); % [%], courbe
p_imm = m.*100./(2.*V_flot.*rho); % [%], points

% axe des y : ligne de flotaison

% calcul des coefficients pour l'équation quadratique : ah^2 + bh + c = 0
a_coef = 2 * rho * L * (B - b) / (2 * H);
b_coef = 2 * rho * L * b;
c_coef = -M;
% calcul des hauteurs d'eau pour chaque poids
Y = (-b_coef + sqrt(b_coef^2 - 4.*a_coef.*c_coef)) ./ (2*a_coef);
y = (-b_coef + sqrt(b_coef^2 - 4.*a_coef.*(-m))) ./ (2*a_coef);

figure;
tiledlayout(1,2);

nexttile;
area(M, 100.*ones(size(M)), 'FaceColor', 'y'); hold on;
plot(M, P_imm, 'c', 'LineWidth', 1.5); hold on; % courbe
for i = 1:length(m)
    plot(p_imm(i).*ones(size(M)), 'b--'); hold on;
end
scatter(m, p_imm, 20, 'r', 'filled'); % points d'intérêt
xlim([0 100]);
ylim([0 100]);
xlabel('Poids (kg)');
ylabel('Portion de volume immergée (%)');
grid on;

nexttile;
area(M, 100.*H.*ones(size(M)), 'FaceColor', 'y'); hold on;
plot(M, 100.*Y, 'c', 'LineWidth', 1.5); hold on; % courbe
for i = 1:length(m)
    plot(100.*y(i).*ones(size(M)), 'b--'); hold on;
end
scatter(m, 100.*y, 20, 'r', 'filled'); % points d'intérêt
xlim([0 100]);
legend('Flotteur');
xlabel('Poids (kg)');
ylabel("Hauteur de l'eau (cm)");