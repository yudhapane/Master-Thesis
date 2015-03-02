function Y = ode4_ti(odefun,tspan,y0,varargin)
%ODE4  Solve time-invariant differential equations with a non-adaptive method of order 4.
%   Y = ODE4(ODEFUN,TSPAN,Y0) with TSPAN = [T1, T2, T3, ... TN] integrates 
%   the system of differential equations y' = f(y) by stepping from T0 to 
%   T1 to TN. Function ODEFUN(Y) must return f(y) in a column vector.
%   The vector Y0 is the initial conditions at T0. Each row in the solution 
%   array Y corresponds to a time specified in TSPAN.
%
%   Y = ODE4(ODEFUN,TSPAN,Y0,P1,P2...) passes the additional parameters 
%   P1,P2... to the derivative function as ODEFUN(T,Y,P1,P2...). 
%
%   This is a non-adaptive solver. The step sequence is determined by TSPAN
%   but the derivative function ODEFUN is evaluated multiple times per step.
%   The solver implements the classical Runge-Kutta method of order 4.   
%
%   Example 
%         tspan = 0:0.1:20;
%         y = ode4(@vdp1,tspan,[2 0]);  
%         plot(tspan,y(:,1));
%     solves the system y' = vdp1(t,y) with a constant step size of 0.1, 
%     and plots the first component of the solution.   
%
% ACKNOWLEDGMENT: This file is derived from the ode4 function, developed by Mathworks, and
% downloaded from:
% http://www.mathworks.com/support/tech-notes/1500/1510.html
%
% Modified by Lucian Busoniu: removed time dependence of odefun, 
% and commented out argument verifications for speed
% Modified by Olivier Sprangers: inserted brackets [] for time dependence
% such that it works with functions that have a time dependence.
%


% if ~isnumeric(tspan)
%   error('TSPAN should be a vector of integration steps.');
% end
% 
% if ~isnumeric(y0)
%   error('Y0 should be a vector of initial conditions.');
% end
% 
h = diff(tspan);
% if any(sign(h(1))*h <= 0)
%   error('Entries of TSPAN are not in order.') 
% end  

% try
%   f0 = feval(odefun,tspan(1),y0,varargin{:});
% catch
%   msg = ['Unable to evaluate the ODEFUN at t0,y0. ',lasterr];
%   error(msg);  
% end  

y0 = y0(:);   % Make a column vector.
% if ~isequal(size(y0),size(f0))
%   error('Inconsistent sizes of Y0 and f(t0,y0).');
% end  

neq = length(y0);
N = length(tspan);
Y = zeros(neq,N);
F = zeros(neq,4);

Y(:,1) = y0;
for i = 2:N
  hi = h(i-1);
  yi = Y(:,i-1);
  F(:,1) = feval(odefun,[],yi,varargin{:});
  F(:,2) = feval(odefun,[],yi+0.5*hi*F(:,1),varargin{:});
  F(:,3) = feval(odefun,[],yi+0.5*hi*F(:,2),varargin{:});  
  F(:,4) = feval(odefun,[],yi+hi*F(:,3),varargin{:});
  Y(:,i) = yi + (hi/6)*(F(:,1) + 2*F(:,2) + 2*F(:,3) + F(:,4));
end
Y = Y.';
