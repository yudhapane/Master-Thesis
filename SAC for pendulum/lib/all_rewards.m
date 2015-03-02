function [rc_cum] = all_rewards(simu,par)
%ALL_REWARDS Collects all reward data
%
% RC_CUM = ALL_REWARDS(SIMU) Collects all the reward data from separate
% learning experiments or simulations and combines the data in one matrix
%
% INPUTS:   SIMU: structure containing number of simulations and simulation
%           name
%
% OUTPUT:   RC_CUM: [#learning experiments x #trials] matrix containing all
%           reward data
cd data
load([simu.name,'_1']); %this will give the parameters

rc_cum  = zeros(simu.no,par.trials);
for i=1:simu.no
%     if(i==7 || i==11|| i==21)
%         simu_name = [simu.name,'_',+num2str(i-1)];    
%     else
        simu_name = [simu.name,'_',+num2str(i)];    
%     end
    load(simu_name)
    rc_cum(i,:) =bk.rc;
end
cd ../