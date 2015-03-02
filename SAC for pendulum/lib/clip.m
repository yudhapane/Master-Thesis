function clip(traj,sys,par)
%CLIP Shows an animated clip of the trajectory of a system
%
% CLIP(TRAJ,SYS,PAR) creates a clip of trajectory TRAJ that system 
% SYS.TYPE has followed.
%
% INPUTS:   SYS: structure that contains SYS.PLOT which is the system that 
%           has followed the trajectory which must be one of the systems in 
%           the folder 'systems'
%           
%           TRAJ: the trajectory the system has followed
%
%           PAR: structure containing simulation parameters. Necessary are
%               - PAR.Tt: simulation time
%               - PAR.Ts: sample time
%               - PAR.SPEED: speed of animation (scalar value >= 0)
%           
%           SPEED: speed of clip
%
% OUTPUT:   Animated clip of trajectory

for i = 1:ceil(par.Tt/par.Ts)
    
   feval(strcat(sys.name,'_plot'),traj(:,i,:),par);
   
   tic
   while toc < par.Ts/par.clipspeed
   
   end
end