function onlineplot(bk,ii,par,sys)
%ONLINEPLOT Returns online plot of learning
%
% ONLINEPLOT(BK, II, PAR) Display online plot of learning. The graph can
% display average reward, parameter errors and temporal difference
%
% INPUTS: PAR: Structure containing simulation settings
%
%         BK: Structure containing bookkeeping variables. See also
%         bookkeeping
%
%         II: Current iteration
%
%         SYS: System parameters
%
% OUTPUT: Online plot of learning
%

if strcmp(par.plotonline,'on')
    % Reward
    figure(1)
    subplot(311)
    plot(linspace(0,(ii*par.Tt)/60,ii), bk.rc(1:ii));
    set(gca, 'xlim', [0 (par.trials*par.Tt)/60]);
    title('Progress');
    xlabel('Time [min]');
    ylabel('Sum of rewards per trial');

    % Parameter convergence
    subplot(312)
    time =0:par.Ts:par.Tt;
    plot(time(1:end-1), squeeze(bk.x_all(ii,:,1)),'b'); grid on;hold on;
    plot(time(1:end-1), squeeze(bk.u(ii,:,1)),'r-.');hold off;%legend('ang','u');
%     plot(linspace(0,(ii*par.Tt)/60,ii), bk.error(1:ii,2),'g'); 
%     if ~isempty(sys.shape)    
%         hold on
%         plot(linspace(0,(ii*par.Tt)/60,ii), bk.error(1:ii,3),'r'); 
%         hold off
%     else
%         hold off
%     end
    set(gca, 'xlim', [0 par.Tt]);
    title('Angle');
    xlabel('Time [sec]');
    ylabel('resp');

    % Temporal difference
    subplot(313)
    plot(linspace(0,(ii*par.Tt)/60,ii), bk.dkc(1:ii));
    set(gca, 'xlim', [0 (par.trials*par.Tt)/60]);
    title('Temporal difference');
    xlabel('Time [min]');
    ylabel('\delta_k');

    set(gcf, 'Name', [sys.name ' - Iteration ' num2str(ii)]);
    drawnow; 
elseif strcmp(par.plotonline,'iter')
    clc
    disp(['Iteration ' num2str(ii)]);
elseif strcmp(par.plotonline,'off')
end
    
    
    
    
    