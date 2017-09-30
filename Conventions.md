* All engine code should be enclosed in the namespace Phoenix.
- Example:
namespace Phoenix
{
	struct Game_t;
}

* Headers use the file ending hpp.
* Implementation files use .cpp.
* If the code is written with the intention to be compiled as C, not C++, use .h and .c.
* Minimize dependencies in headers, i.e. make liberal use of forward declarations. 
* Consider using pimpl in non-critical code to minimize definitions leakage.
* Generally split declarations and defenitions, i.e. prefer only using declarations in headers.

* Classes begin with an uppercase letter.
* Generally, classes should list their public, then protected, then private members, in this order.
* Minimize the use of inheritance. 
* Use structs for POD types, i.e. types containing only public data and no methods.
* Member variable names generally begin with "m_", this may be omitted for pod members.
* Member methods begin with a lowercase letter.
* Free functions begin with a lowercase letter.
* Always prefer const implementations if possible. This promotes a style less dependent on internal state
  and makes reasoning about data flow easier.
* Prefer explicit ctors.
* Interface classes, which contain atleast one pure virtual method and a virtual dtor begin with the letter "I".
* Prefer entirely pure interfaces, i.e. containing no data members and only pure virtual methods.
* Always use the override specifier when implementing virtual methods.
- Example:
struct Task
{
	void* data;
	size_t size;
}

void processTask(Task* task);

template <class T>
class FancyTask
{
	public:
		T* getAsT();
		const T* getAsT() const;
		
	private:
		void* m_data;
		size_t m_size*
}

class IFile
{
	public:
		virtual ~IFile();
		virtual void open() = 0;
		virtual void close() = 0;
}

class WinFile : public IFile
{
	public:
		virtual void open() override;
		virtual void close() override;
}


* Enums begin with the letter E and generally use the pattern of namespace:"Enum name", enum:"Thing enum describes"
- Example:
namespace EUniform
{
	enum Type
	{
		// Enum Types.
	}
}

* Prefer stdint.h types over regular built-in number types.
* Use size_t for representing sizes, especially when referring to memory. This includes indexing arrays. 
- Example:

struct ActorStats
{
	uint32_t maxActors;
	uint32_t actorsInUse;
}

Actor* getActor(size_t id)
{
	return actors[id];
}

void* allocMemoryPool(size_t numBlocks, size_t blockSize);


