function G = DelMiddleNode(G)

%fem una copia del graph per tractar cada grup de nodes intermitjos
G1 = G;
G2 = G;

%% 1. Eliminem clicles que estan a les fulles
%dirigim el graph
nodesTract=[1];
while ~isempty(nodesTract)
    adjNew = Graph2dirGraph(G1.e);
    %busquem nodes amb dos pares o més
    nodesAmb2Pares = find((sum(adjNew>0,1)>=2)>0);
    nodesAmb2Pares = unique(nodesAmb2Pares);
    %hd = view(biograph(adjNew));
    %set(hd.nodes(nodesDir),'Color',[1 0 0]);
    %busquem els nodes fulla
    %nodesFulla=find(G1.wv==0);
    %nodes sense fills
    nodesSenseFills = find((sum(adjNew>0,2)==0)>0);
    %nodes amb dos pares que no son fulla
    %nodesTract = setdiff(nodesAmb2Pares,nodesFulla);
    %nodesTract = nodesDir;
    %nodes amb dos pares que no tenen fills
    nodesTract = intersect(nodesAmb2Pares,nodesSenseFills);
    %eliminem aquests nodes
    G1.e(:,nodesTract) = 0;
    G1.e(nodesTract,:) = 0;
    %eliminem totes les conexions dels nodes eliminats i els nodes als que es
    %conecten junt amb les seves conexions tb
    for k=1:length(nodesTract)
        conn = find(adjNew(:,nodesTract(k))>0);
        %G1.e(:,conn) = 0;
        %G1.e(conn,:) = 0;
        for k1=1:length(conn)
            G1.we{conn(k1),nodesTract(k)} = [];
            G1.we{nodesTract(k),conn(k1)} = [];
            %conn2 = find(adjNew(:,conn(k1))>0);
            %for k2=1:length(conn2)
            %    G1.we{conn(k1),conn2(k2)} = [];
            %    G1.we{conn2(k2),conn(k1)} = [];
            %end
        end 
    end
end
%%%fins que nodesTract estigui buit!!!!
GPrimerPas = G1;


% %calculem propietats dels nodes (Nivell)
% [NodeProps,NodePropsSup]=AdjMat2NodeProps(adjNew,1);
% 
% 
% %%%% amb això podem mirar sobre certs nodes si els seus pares tenen
% %%%% diferent nivell
% parent=[];
% nodeKeep =[];
% nodeKeepLevel = [];
% for k=1:length(nodesTract)
%     parent = find(adjNew(:,nodesTract(k))>0);
%     for k1=1:length(parent)
%       nodeKeepLevel(k1)=NodeProps(parent(k1)).Level;
%     end
%     if min(nodeKeepLevel)~=max(nodeKeepLevel)
%         [sd,vs]= min(nodeKeepLevel);
%         %eliminem el link que te mes diferencia de nivell de la matriu i
%         %treiem els punts que el composen
%         G1.e(parent(vs),nodesTract(k)) = 0; 
%         G1.e(nodesTract(k),parent(vs)) = 0;
%         G1.we{parent(vs),nodesTract(k)} = [];
%         G1.we{nodesTract(k),parent(vs)} = [];
%     end
% end
% 
% %%%%%%%%%%%%%%%%



%% 2 MidNodes

%busquem els nodes dordre 2
nodes=[1];
CondNodes=~isempty(nodes);
CondInf=1;

while (CondNodes.*CondInf)
    G2 = G1;
    GAnterior = G1;
    nodes0=nodes;
    nodes = find((sum(GAnterior.e>0,2)==2)>0);
    nodes = unique(nodes);
    CondInf=~isempty(setdiff(nodes,nodes0));
    CondNodes=~isempty(nodes);
  
    
   
    %Busquem els lligams amb els extrems
    conn= find(any(GAnterior.e(nodes,:),1));
    conn2 = setdiff(unique(conn),nodes);
    %eliminem els lligams dels nodes intermitjos amb els extrems, posem a 0
    G1.e(nodes,conn2) = 0; 
    G1.e(conn2,nodes) = 0;
    %busquem les components connexes
    [ci sizes] = components(sparse(G1.e));
    %per cada component connexa tractem els nodes si son intermitjos
    for i=1:size(sizes,1)
        indx = find(ci==i);
        %si un dels nodes pertany a intermitjos els tractem
        if find(nodes==indx(1))
         connMidNode= find(any(GAnterior.e(indx,:),1));
         connMidNode2 = setdiff(unique(connMidNode),indx); 
         %Al afegir el nou enllaç Possible problema: un únic extrem ( no els
         %tractem)
         if numel(connMidNode2)>1
             %treure les connexions amb aquests dos nodes i afegir el nou enllaç 
             G2.e(:,indx) = 0;
             G2.e(indx,:) = 0;
             G2.e(connMidNode2(1),connMidNode2(2))=1;
             G2.e(connMidNode2(2),connMidNode2(1))=1;
             %juntem els extrems amb els punts que el composen
             %Per cada node intermig afegim els lligams que tingui a la relacio
             %dels nodes extrems
             for j=1:numel(indx)   
                 lligams = find(G.e(indx(j),:)>0); 
                 for k=1:numel(lligams)
                     if j==1 && k==1
                         G2.we{connMidNode2(1),connMidNode2(2)} = [G2.we{indx(j),lligams(k)}];
                         G2.we{connMidNode2(2),connMidNode2(1)} = [G2.we{indx(j),lligams(k)}];
                     else
                         G2.we{connMidNode2(1),connMidNode2(2)} = [G2.we{connMidNode2(1),connMidNode2(2)},G2.we{indx(j),lligams(k)}];
                         G2.we{connMidNode2(2),connMidNode2(1)} = [G2.we{connMidNode2(1),connMidNode2(2)},G2.we{indx(j),lligams(k)}];
                     end
                 end         
             end
         %mid nodes amb un unic extrem treiem link de menys nivell
         else

         end
        end   
    end
    G1 = G2;

end

GSegonPas=G1;
G=GSegonPas;


% hf = view(biograph(G.e));
% set(hf.nodes(unique(nodes)),'Color',[1 0 0])
% set(hf.nodes(unique(conn2)),'Color',[0 1 0])
end