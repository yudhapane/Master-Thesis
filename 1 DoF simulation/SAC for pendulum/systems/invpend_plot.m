function invpend_plot(x,par)
%INVPEND_PLOT plots of invpend set-up for a given state x
%
% INVPEND_PLOT(X,PAR) plots a figure of an inverted pendulum system in a state
% x
%
% INPUTS:   X: [2 x 1] state vector
%           PAR: structure containing simulation parameters. At least
%           PAR.RADIUSPLOT is required for the radius of the plot.
%
% OUTPUT:   Figure of inverted pendulum system

% Clear current figure
figure(1);
clf
hold on

% Determine position
x1 = sin(x(1));
y1 = cos(x(1));

% Plot inverted pendulum
plot([0;x1],[0;y1],'Linewidth',3)
plot(0,0,'k.','Markersize',30)
plot(x1,y1,'r.','Markersize',50)
axis([-par.radiusplot par.radiusplot -par.radiusplot par.radiusplot])
hold off