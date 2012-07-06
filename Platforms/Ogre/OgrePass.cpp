#include "OgrePass.hpp"

#include <OgrePass.h>
#include <OgreTechnique.h>

#include "OgreTextureUnitState.hpp"
#include "OgreGpuProgram.hpp"
#include "OgreMaterial.hpp"
#include "OgreMaterialSerializer.hpp"
#include "OgrePlatform.hpp"

namespace sh
{
	OgrePass::OgrePass (OgreMaterial* parent, const std::string& configuration)
		: Pass()
	{
		Ogre::Technique* t = parent->getOgreMaterial()->getTechnique(configuration);
		mPass = t->createPass();
	}

	boost::shared_ptr<TextureUnitState> OgrePass::createTextureUnitState ()
	{
		return boost::shared_ptr<TextureUnitState> (new OgreTextureUnitState (this));
	}

	void OgrePass::assignProgram (GpuProgramType type, const std::string& name)
	{
		if (type == GPT_Vertex)
			mPass->setVertexProgram (name);
		else if (type == GPT_Fragment)
			mPass->setFragmentProgram (name);
		else
			throw std::runtime_error("unsupported GpuProgramType");
	}

	Ogre::Pass* OgrePass::getOgrePass ()
	{
		return mPass;
	}

	bool OgrePass::setPropertyOverride (const std::string &name, PropertyValuePtr& value, PropertySetGet* context)
	{
		if (((typeid(*value) == typeid(StringValue)) || typeid(*value) == typeid(LinkedValue))
				&& retrieveValue<StringValue>(value, context).get() == "default")
			return true;

		if (name == "vertex_program")
			return true; // handled already
		else if (name == "fragment_program")
			return true; // handled already
		else if (name == "ffp_vertex_colour_ambient")
		{
			bool enabled = retrieveValue<BooleanValue>(value, context).get();
			// fixed-function vertex colour tracking
			mPass->setVertexColourTracking(enabled ? Ogre::TVC_AMBIENT : Ogre::TVC_NONE);
			return true;
		}
		else
		{
			OgreMaterialSerializer& s = OgrePlatform::getSerializer();

			return s.setPassProperty (name, retrieveValue<StringValue>(value, context).get(), mPass);
		}
	}

	void OgrePass::setGpuConstant (int type, const std::string& name, ValueType vt, PropertyValuePtr value, PropertySetGet* context)
	{
		Ogre::GpuProgramParametersSharedPtr params;
		if (type == GPT_Vertex)
			params = mPass->getVertexProgramParameters();
		else if (type == GPT_Fragment)
			params = mPass->getFragmentProgramParameters();

		if (vt == VT_Float)
			params->setNamedConstant (name, retrieveValue<FloatValue>(value, context).get());
		else if (vt == VT_Int)
			params->setNamedConstant (name, retrieveValue<IntValue>(value, context).get());
		else if (vt == VT_Vector4)
		{
			Vector4 v = retrieveValue<Vector4>(value, context);
			params->setNamedConstant (name, Ogre::Vector4(v.mX, v.mY, v.mZ, v.mW));
		}
		else if (vt == VT_Vector3)
		{
			Vector3 v = retrieveValue<Vector3>(value, context);
			params->setNamedConstant (name, Ogre::Vector4(v.mX, v.mY, v.mZ, 1.0));
		}
		else if (vt == VT_Vector2)
		{
			Vector2 v = retrieveValue<Vector2>(value, context);
			params->setNamedConstant (name, Ogre::Vector4(v.mX, v.mY, 1.0, 1.0));
		}
		else
			throw std::runtime_error ("unsupported constant type");
	}

	void OgrePass::addSharedParameter (int type, const std::string& name)
	{
		Ogre::GpuProgramParametersSharedPtr params;
		if (type == GPT_Vertex)
			params = mPass->getVertexProgramParameters();
		else if (type == GPT_Fragment)
			params = mPass->getFragmentProgramParameters();

		params->addSharedParameters (name);
	}
}
