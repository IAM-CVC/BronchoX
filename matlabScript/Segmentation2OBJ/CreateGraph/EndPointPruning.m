function G = EndPointPruning(GSegonPas)

%dirigim el graph per trobar els nodes que no tenen fills
adjNew = Graph2dirGraph(GSegonPas.e);
nodesSenseFills = find((sum(adjNew>0,2)==0)>0);
%busquem els nodes que no son fulla
nodesNoFulla = find(GSegonPas.wv(:)==1);
%fem la intersecció per trobar les potes no vàlides
nodesElim = intersect(nodesSenseFills,nodesNoFulla);
%eliminem aquests nodes
conn= find(any(GSegonPas.e(nodesElim,:),1));
GSegonPas.e(:,nodesElim) = 0;
       GSegonPas.e(nodesElim,:) = 0;
        for k1=1:length(nodesElim)
            for k2=1:length(conn)
            GSegonPas.we{nodesElim(k1),conn(k2)} = [];
            GSegonPas.we{conn(k2),nodesElim(k1)} = [];
            end
        end
G= GSegonPas;

end