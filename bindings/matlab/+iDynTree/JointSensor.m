classdef JointSensor < iDynTree.Sensor
  methods
    function delete(self)
      if self.swigPtr
        iDynTreeMEX(973, self);
        self.swigPtr=[];
      end
    end
    function varargout = getParentJoint(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(974, self, varargin{:});
    end
    function varargout = getParentJointIndex(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(975, self, varargin{:});
    end
    function varargout = setParentJoint(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(976, self, varargin{:});
    end
    function varargout = setParentJointIndex(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(977, self, varargin{:});
    end
    function self = JointSensor(varargin)
      self@iDynTree.Sensor(SwigRef.Null);
      if nargin==1 && strcmp(class(varargin{1}),'SwigRef')
        if ~isnull(varargin{1})
          self.swigPtr = varargin{1}.swigPtr;
        end
      else
        error('No matching constructor');
      end
    end
  end
  methods(Static)
  end
end
