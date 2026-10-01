function G=Cycle3nodesTract(G)

GPrimerPasCopia = G;
[n,circ] = find_elem_circuits(GPrimerPasCopia.e);

if n~=0
    nodes=unique([circ{1,:}]);
    %dirigim el graph per trobar els nodes que no tenen fills
    adjNew = Graph2dirGraph(GPrimerPasCopia.e);
    nodesSenseFills = find((sum(adjNew>0,2)==0)>0);
    %calculem propietats dels nodes (Nivell)
    [NodeProps,NodePropsSup]=AdjMat2NodeProps(adjNew,1);

    ciclesPoda=circ;
    for i=1:numel(circ)
       tocafulles=[];
       cicle= unique(circ{i});
       %busquem tots els nodes que el conecten
       conn= find(any(GPrimerPasCopia.e(cicle,:),1));
       %si toca alguna fulla (no fills) o és un cicle de més de 3 eliminem el cicle
       tocafulles= intersect(conn,nodesSenseFills);
       %if (isempty(tocafulles)&&(numel(cicle)<=3)) %3 pq hem fet un unique
       if (numel(cicle)<=3) %3 pq hem fet un unique
           %2 cas de triangle a la bifucació
           nivells=  [NodeProps(cicle).Level];
           [vx,ix]= min(nivells);
           elimconn=setdiff(cicle,cicle(ix));
           %eliminem les connexions dels de nivell inferior pero només entre
           %ells
           GPrimerPasCopia.e(elimconn,elimconn) = 0;
           GPrimerPasCopia.e(elimconn,elimconn) = 0;
           for k1=1:length(elimconn)
                for k2=1:length(elimconn)
                GPrimerPasCopia.we{elimconn(k1),elimconn(k2)} = [];
                GPrimerPasCopia.we{elimconn(k2),elimconn(k1)} = [];
                end
           end  
       end   
    end
end
G=GPrimerPasCopia;