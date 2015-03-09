function bk = bookkeeping(x,par,sys)
%BOOKKEEPING Generates bookkeeping matrices
%
% BK = BOOKKEEPING(X,PAR,SYS) generates bookkeeping variables for the
% various algorithms
%
% INPUTS:   X: [n x 1] state of the system
%           
%           PAR: structure containing at least # trials (par.trials), sample time
%           (par.Ts) and simtime (par.Tt)
%           
%           SYS: structure containing system information. At least
%           sys.shape is necessary to determine the size of the actor-error
%           vector
%
% OUTPUTS:  BK Structure containing
%               - RC: Matrix for cumulative rewards
%               - DKC: Matrix for cumulative temporal difference
%               - ERROR: Matrix for errors on actor(s) and critic
%               - X_ALL: Matrix containing all visited states

bk.rc           = zeros(par.trials,1);
bk.dkc          = zeros(par.trials,1);
bk.x_all        = zeros(par.trials,par.Ns,length(x));
bk.uc        = zeros(par.trials,par.Ns,length(x));
bk.u        = zeros(par.trials,par.Ns,length(x));
bk.Delta_u        = zeros(par.trials,par.Ns,length(x));
bk.Hd = zeros(par.trials,par.Ns);
bk.Rd = zeros(par.trials,par.Ns);
% if isfield(par,'aDIBF')
%     if strcmp(par.aDIBF.type,'four')
%         bk.polgrad.di   = zeros(par.trials,par.Ns,length(par.aDIBF.pb));
%     elseif strcmp(par.aDIBF.type,'rbf')
%         bk.polgrad.di   = zeros(par.trials,par.Ns,prod(par.aDIBF.N));
%     end
%     if strcmp(par.aESBF.type,'four')
%         bk.polgrad.es   = zeros(par.trials,par.Ns,length(par.aESBF.pb));
%     elseif strcmp(par.aESBF.type,'rbf')
%         bk.polgrad.di   = zeros(par.trials,par.Ns,prod(par.aESBF.N));
%     end
% end