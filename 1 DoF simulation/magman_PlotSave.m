function [] = magman_PlotSave(bk,par,sys)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
SumRewards=sum(bk.rP);
[~, max_kk] = max(SumRewards(:));
%%
RBF3D(par.RBF.c,bk.theta{max_kk},bk.xi{max_kk},3);

% Simulate final response
magman_SimFinal(par,sys,bk.xi{max_kk},max_kk);
% Plot initial
magman_plotResponse(par,bk,1,SumRewards(1))            % initial response

% Plot Best
%magman_plotResponse(par,bk,max_kk,SumRewards(max_kk))  % plot response with max reward 

figure
plot(SumRewards)
title('Sum of Rewards for each trial nr')
% plot(bk.dk)
% title('Temporal difference for each trial')

cd('D:\jdamsteeg\Dropbox\Magman\Matlab\Control_simulation\RL\SavePlots')
button = questdlg('Figuren en variablen opslaan?');
if length(button)==3
h = get(0,'children');
c = clock;
for i=1:length(h)
  saveas(h(i), ['Sim' '-' num2str(c(3)) '-' num2str(c(2)) ' ' num2str(c(4)) 'u' num2str(c(5)) 'plot' num2str(i)], 'fig');
end
save(['Sim' '-' num2str(c(3)) '-' num2str(c(2)) ' ' num2str(c(4)) 'u' num2str(c(5))],'par')
end

end

