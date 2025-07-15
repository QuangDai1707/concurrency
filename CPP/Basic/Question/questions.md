# 1. The Rule of Three
- if class manage resources, need define:
    - Destructor
    - Copy Constructor
    - Copy Assignment Operator
Because the compiler's default implementations won’t handle deep copies of resources correctly — which can lead to double deletes, memory leaks, or shallow copy bugs.

# 2. Difference between vector and raw array
- Memory management
    - Raw: not, need handle alloc and deallow manually
    - Vector: size can grow/shrink
- safety:
    - Raw: can be out of bound to undefined behaviour (access to random memory location)
    - Vector: throw exeption if out of bound
- feature:
    - raw: dont have much, only access operator []
    - vector: many: push_back, size, clear

# 3. What is smart pointer? difference between unique, shared and weak pointer
- smart pointer: wrapper class of raw pointer, manage lifetime of object it point to (dont need to delete manually)
- difference:
    - unique: 
        - one pointer own object at a time
        - cannot copy object but can move (transfer ownership)
        - automatic delete object if pointer go out of scope
    - shared:
        - multiple pointers can have ownership on 1 object
        - internally keep the reference count
        - delete object when last pointer go out of scope
    - weak:
        - do not own object - hold non-owning reference to object manage by share
        - Useful to break cyclic references that cause memory leaks.


# 4. what is polymorphism
- polymorphism allows different behaviors via the same interface
- The method call is resolved at runtime, based on the actual object type.
- If you don’t use virtual, C++ doesn't know you want polymorphic behavior — it assumes performance is more important and resolves the function call at compile time.

kiến thức ngôn ngữ:
oop: vì sao lại có 4 tính chất đó
đóng gói: vì sao
hỏi đặc thù về ngôn ngữ: 
c++
chuyên sâu c++
cho tình huống:
tư duy giải quyết vấn đề
xử lý vấn đề như thế nào
thuật toán:
code