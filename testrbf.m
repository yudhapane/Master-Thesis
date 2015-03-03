N = 100; % discretization
x1 = linspace(-5,5,N);
x2 = linspace(-5,5,N);
X1 = repmat(x1, [N,1]);
X1v = reshape(X1, [1, N*N]);
X2v = repmat(x2, [1, N]);
X = [X1v; X2v];

Xd = X1;
Yd = repmat(transpose(x2), [1,N]);
c = [0; 0];
c = repmat(c,[1,N*N]);
B = [100 0; 0 50];
% X1 = repmat(x(1,:), [N, 1]);
% X2 = repmat(transpose(x(2,:)), [1, N]);

tic
temp = (X-c)'*inv(B);
temp2 = temp.*transpose(X-c);
Phi = exp(-0.5*sum(temp2,2));

% Phi = exp(-0.5*(X-c)'*inv(B)*(X-c));
% Phi = diag(Phi);
Phi = reshape(Phi,[N,N]);
toc
figure;
surf(Xd,Yd,Phi)

