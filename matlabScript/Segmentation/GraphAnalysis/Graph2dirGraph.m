function adjNew = Graph2dirGraph(adjGraph,varargin)

adjNew = adjGraph;
pare=[1];
if(~isempty(varargin))
    pare=varargin{1};
end
child = pare;
pervisitar = pare;
visited=[];
Nnode=size(adjGraph,1);
it=1;
%% FALTA EL PARAMETRE DE STOP!
while ((size(visited,2)~=Nnode).*(it<=10*Nnode))
        if numel(pervisitar)~=0,pare= pervisitar(1);end;
        [child] = find(adjGraph(pare,:)>0);
        child = setdiff(child,visited);
        for j=1:numel(child)
            adjNew(child(j),pare) = 0;
            pervisitar = [pervisitar child(j)];
        end
        if numel(pervisitar)~=0, pervisitar(1) = [];end;
        visited=unique([visited,pare]);
        it=it+1;
end

end