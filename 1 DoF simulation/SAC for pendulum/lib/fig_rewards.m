function fig_rewards(rc_cum, par)
%FIG_REWARDS Figure of all cumulative rewards
%
% FIG_REWARDS(SIMU) Creates figure of cumulative rewards
%
% INPUTS:   SIMU: structure containing number of simulations and simulation
%           name
%
% OUTPUT:   Figure of average reward over all simulations including min,
%           max, confidence region and average


% Average reward over all experiments
[~, ~, muci, ~] = normfit(rc_cum);
sc              = linspace(0,(par.trials*par.Tt)/60,par.trials);
xdata           = [sc,fliplr(sc)];
filled          = [muci(1,:),fliplr(muci(2,:))];

%     figure(1);
    fill(xdata,filled,[1/1.5,1/1.5,1/1.5],'EdgeColor',[1/1.5,1/1.5,1/1.5]); hold on
    plot(sc, mean(rc_cum),'k'); hold on
    plot(sc, [max(rc_cum);min(rc_cum)],'k--'); hold off
    xlabel('Time [min]')
    ylabel('Sum of rewards per trial')
    legend('95% confidence region for the mean','Mean','Max and min bounds','Location','SouthEast')
    axis([0 max(sc) min(min(rc_cum)) 0])
