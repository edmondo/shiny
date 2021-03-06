#ifndef SH_MATERIALINSTANCE_H
#define SH_MATERIALINSTANCE_H

#include <vector>

#include "PropertyBase.hpp"
#include "Platform.hpp"
#include "MaterialInstancePass.hpp"

namespace sh
{
	class Factory;

	typedef std::vector<MaterialInstancePass> PassVector;

	/**
	 * @brief
	 * Allows you to be notified when a certain configuration for a material was just about to be created. \n
	 * Useful for adjusting some properties prior to the material being created (Or you could also re-create
	 * the whole material from scratch, i.e. use this as a method to create this material entirely in code)
	 */
	class MaterialInstanceListener
	{
	public:
		virtual void requestedConfiguration (MaterialInstance* m, const std::string& configuration) = 0; ///< called before creating
		virtual void createdConfiguration (MaterialInstance* m, const std::string& configuration) = 0; ///< called after creating
	};

	/**
	 * @brief
	 * A specific material instance, which has all required properties set
	 * (for example the diffuse & normal map, ambient/diffuse/specular values). \n
	 * Depending on these properties, the system will automatically select a shader permutation
	 * that suits these and create the backend materials / passes (provided by the \a Platform class).
	 */
	class MaterialInstance : public PropertySetGet
	{
	public:
		MaterialInstance (const std::string& name, Factory* f);
		virtual ~MaterialInstance ();

		MaterialInstancePass* createPass ();
		PassVector getPasses(); ///< gets the passes of the top-most parent

		/// @attention Because the backend material passes are created on demand, the returned material here might not contain anything yet!
		/// The only place where you should use this method, is for the MaterialInstance given by the MaterialListener::materialCreated event!
		Material* getMaterial();

		/// attach a \a MaterialInstanceListener to this specific material (as opposed to \a MaterialListener, which listens to all materials)
		void setListener (MaterialInstanceListener* l) { mListener = l; }

		std::string getName() { return mName; }

		virtual void setProperty (const std::string& name, PropertyValuePtr value);

	private:
		void setParentInstance (const std::string& name);
		std::string getParentInstance ();

		void create (Platform* platform);
		void createForConfiguration (const std::string& configuration, unsigned short lodIndex);

		void destroyAll ();

		void setShadersEnabled (bool enabled);

		friend class Factory;


	private:
		std::string mParentInstance;
		///< this is only used during the file-loading phase. an instance could be loaded before its parent is loaded,
		/// so initially only the parent's name is written to this member.
		/// once all instances are loaded, the actual mParent pointer (from PropertySetGet class) can be set

		std::vector< boost::shared_ptr<TextureUnitState> > mTexUnits;

		MaterialInstanceListener* mListener;

		PassVector mPasses;

		std::string mName;

		boost::shared_ptr<Material> mMaterial;

		bool mShadersEnabled;

		Factory* mFactory;
	};
}

#endif
