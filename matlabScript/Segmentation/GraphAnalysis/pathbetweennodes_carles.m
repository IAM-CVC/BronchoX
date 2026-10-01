function [pth,npahtsToNodes,pahtsToNodes] = pathbetweennodes_carles(adj, src, snk, endNodes , verbose)
%PATHBETWEENNODES Return all paths between two nodes of a graph
%
% pth = pathbetweennodes(adj, src, snk)
% pth = pathbetweennodes(adj, src, snk, vflag)
%
%
% This function returns all simple paths (i.e. no cycles) between two nodes
% in a graph.  Not sure this is the most efficient algorithm, but it seems
% to work quickly for small graphs, and isn't too terrible for graphs with
% ~50 nodes.
%
% Input variables:
%
%   adj:    adjacency matrix
%
%   src:    index of starting node
%
%   snk:    index of target node
%
%   vflag:  logical scalar for verbose mode.  If true, prints paths to
%           screen as it traverses them (can be useful for larger,
%           time-consuming graphs). [false]
%
% Output variables:
%
%   pth:    cell array, with each cell holding the indices of a unique path
%           of nodes from src to snk.

% Copyright 2014 Kelly Kearney

%%% new carles %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
npahtsToNodes = zeros(size(adj,2),1);
pahtsToNodes ={};
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

if nargin < 4
    verbose = false;
end

n = size(adj,1);

stack = src;

stop = false;

pth = cell(0);
cycles = cell(0);

next = cell(n,1);
for in = 1:n
    next{in} = find(adj(in,:));
end

visited = cell(0);

pred = src;

Nit=1;
ItMax=2000;

%% Deb Modification: Set maximum number of iterations for complex grafs
%while 1
while Nit<ItMax
    
    visited = [visited; sprintf('%d,', stack)];
    
    [stack, pred] = addnode(stack, next, visited, pred);
    
    %%% new carles %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    if ~isempty(stack)
        nodeid = find(endNodes==stack(end));
        if ~isempty(nodeid)
            if (npahtsToNodes(endNodes(nodeid))~=0)
                if (size(pahtsToNodes{endNodes(nodeid),npahtsToNodes(endNodes(nodeid))},2)==size(stack,2))
                    resta = pahtsToNodes{endNodes(nodeid),npahtsToNodes(endNodes(nodeid))} - stack;
                    if(min(resta)~=0)
                        npahtsToNodes(endNodes(nodeid))=npahtsToNodes(endNodes(nodeid))+1;
                        pahtsToNodes{endNodes(nodeid),npahtsToNodes(endNodes(nodeid))}=stack;
                    end
                else
                    npahtsToNodes(endNodes(nodeid))=npahtsToNodes(endNodes(nodeid))+1;
                    pahtsToNodes{endNodes(nodeid),npahtsToNodes(endNodes(nodeid))}=stack;
                end
            else
                npahtsToNodes(endNodes(nodeid))=npahtsToNodes(endNodes(nodeid))+1;
                pahtsToNodes{endNodes(nodeid),npahtsToNodes(endNodes(nodeid))}=stack;
            end
        end
    end
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    if verbose
        fprintf('%2d ', stack);
        fprintf('\n');
    end
    %     if numel(stack)==1
    %     e=1;
    %     end
    if isempty(stack)
        break;
    end
    
    
    if stack(end) == snk
        pth = [pth; {stack}];
        visited = [visited; sprintf('%d,', stack)];
        stack = popnode(stack);
    elseif length(unique(stack)) < length(stack)
        cycles = [cycles; {stack}];
        visited = [visited; sprintf('%d,', stack)];
        stack = popnode(stack);
    end
    
    %% Deb Modification
    Nit=Nit+1;
end


if(Nit==ItMax)
    
    npahtsToNodes=Inf;
    pahtsToNodes={};
    pth={};
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [stack, pred] = addnode(stack, next, visited, pred)

newnode = setdiff(next{stack(end)}, pred);
possible = arrayfun(@(x) sprintf('%d,', [stack x]), newnode, 'uni', 0);

isnew = ~ismember(possible, visited);

if any(isnew)
    idx = find(isnew, 1);
    stack = str2num(possible{idx});
    pred = stack(end-1);
else
    [stack, pred] = popnode(stack);
end


function [stack, pred] = popnode(stack)

stack = stack(1:end-1);
if length(stack) > 1
    pred = stack(end-1);
else
    pred = [];
end