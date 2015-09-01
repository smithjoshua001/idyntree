classdef LinkVelAcc < SwigRef
  methods
    function varargout = vel(self,varargin)
      [varargout{1:max(1,nargout)}] = iDynTreeMATLAB_wrap(343, self, varargin{:});
    end
    function varargout = acc(self,varargin)
      [varargout{1:max(1,nargout)}] = iDynTreeMATLAB_wrap(344, self, varargin{:});
    end
    function delete(self)
      if self.swigInd
        iDynTreeMATLAB_wrap(345, self);
        self.swigInd=uint64(0);
      end
    end
    function self = LinkVelAcc(varargin)
      if nargin~=1 || ~ischar(varargin{1}) || ~strcmp(varargin{1},'_swigCreate')
        % How to get working on C side? Commented out, replaed by hack below
        %self.swigInd = iDynTreeMATLAB_wrap(346, varargin{:});
        tmp = iDynTreeMATLAB_wrap(346, varargin{:}); % FIXME
        self.swigInd = tmp.swigInd;
        tmp.swigInd = uint64(0);
      end
    end
  end
  methods(Static)
  end
end