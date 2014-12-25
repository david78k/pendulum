% scaraplot.m
%
% $Id: scaraplot.m,v 1.1 2006/05/04 17:19:28 jdlope Exp $

% Visualiza las posiciones de cada una de las articulaciones en 3D.

function scaraplot(t1,t2,t3,d4,goal)
    global grafica
    p = scarapos(t1,t2,t3,d4);    
    t = 0:pi/10:2*pi;
    C = zeros(1,size(t,2))+ 0.6;
    %colormap cool
    
        
    [X1,Y1,Z1] = cylinder(3,7);
    s1 =surf(X1,Y1,Z1*4,'FaceColor',[0.9 0.9 0.9]);
    
    hold on
    %goal
    plot3(goal(1),goal(2),goal(3),'.','markersize',20,'MarkerEdgeColor','r');
    
    plot3(p(:,1),p(:,2),p(:,3),'.-','Color',[0.4 0.4 0.4],'LineWidth',3,'markersize',30,'MarkerEdgeColor',[0.7 0.7 0.7]);
    plot3(p(:,1),p(:,2),p(:,3),'*','Color',[0.0 0.0 0.0],'markersize',7,'MarkerEdgeColor',[0.0 0.0 0.0]);
   %plot3(trace(:,1),trace(:,2),trace(:,3),'Color',[0.7 0.7 0.7]);
    %text(p(1,1),p(1,2),p(1,3)+1,['t1: ',num2str(t1)]);
    %text(p(2,1),p(2,2),p(2,3)+1,['t2: ',num2str(t2)]);
    %text(p(4,1),p(4,2),p(4,3)-1,['t4: ',num2str(d4)]);
   
    axis([ -50 50  -50 50  0 6]);    
    if (grafica==true)
        %view(45,30)
        view(90,90)
        grid on
        box off
    else
        view(90,90)
        grid off
        box on
    end
    %axis off;
    %box on
    set(gcf, 'color', 'white');
    
    hold off
