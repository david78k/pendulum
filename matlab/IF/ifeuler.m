dt=2;
tend=100;
E=-65;
R=1;
I=20;
Vth=-50;
Vreset=-55;
tau=5;
u=E;
n=round(tend/dt);
usafe=zeros(n,1);
for i=1:n,
    usafe(i)=u;
    dudt=(E+R*I-u)/tau;
    u=u+dudt*dt;
    if u>Vth,
        u=Vreset;
    end
end
plot([1:n]*dt,usafe);