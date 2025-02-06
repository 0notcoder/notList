#pragma once
#include <memory>
#include <stdio.h>

typedef unsigned long long notSize_t;

template <typename T>
class CNotList final {
private:
	class CNotListNode final {
	public:
		T m_value;
		CNotListNode *pNext;
		CNotListNode *pPrev;

		CNotListNode(const T &TValue,
			CNotListNode *pNext=nullptr, CNotListNode *pPrev=nullptr) :
			m_value(TValue), pNext(pNext), pPrev(pPrev) {};

		void vSetMembers(const T &, CNotListNode *pNext = nullptr, CNotListNode *pPrev = nullptr);
		
	};

	notSize_t ullSize;
	CNotListNode *pHead;
	CNotListNode *pTail;

	CNotListNode *pGetElementByIndex(const notSize_t&) const;
	static CNotListNode *pAllocateNewElement();
	static CNotListNode *pAllocateAndInitializeNewElement(const T&,
		CNotListNode* pNext=nullptr, CNotListNode* pPrev=nullptr);
	static CNotListNode *pCopyNodeFromAnother(CNotListNode *);
	void vRemoveElementByPointer(CNotListNode *);

public:
	CNotList() : ullSize(0), pHead(nullptr), pTail(nullptr) {};
	CNotList(const CNotList<T>&);
	CNotList(CNotList<T> &&) noexcept = delete;
	~CNotList();

	void operator=(const CNotList<T> &rOtherList);
	void vPush_back(const T&);
	void vInsert(const notSize_t &, const T&);
	
	void vRemoveElement(const notSize_t &);
	notSize_t ullGetSize() const;
	void vReserve(const notSize_t &);
	T operator[](const notSize_t&) const;
};

// Implementations:

template <typename T>
CNotList<T>::CNotList(const CNotList<T> &rOtherList) : CNotList() {
	this->operator=(rOtherList);
}

template <typename T>
void CNotList<T>::operator=(const CNotList<T> &rOtherList) {
	for (notSize_t ullI = 0; ullI < rOtherList.ullSize; ullI++) {
		this->vPush_back(rOtherList[ullI]);
	}
}

template <typename T>
void CNotList<T>::vPush_back(const T &TValue) {
	CNotListNode *pNewElement = pAllocateAndInitializeNewElement(TValue);

	if (ullSize == 0) {
		pHead = pNewElement;
		pTail = pHead;
	}
	else if (ullSize == 1) {
		pTail = pNewElement;
		pTail->pPrev = pHead;
		pHead->pNext = pTail;
	}
	else {
		CNotListNode *pPrevTail = pTail;
		pTail = pNewElement;
		pTail->pPrev = pPrevTail;
		pPrevTail->pNext = pTail;
	}

	++ullSize;
}



template<typename T>
void CNotList<T>::vInsert(const notSize_t &ullIndex, const T &TValue) {
	if (ullIndex == ullSize) {
		this->vPush_back(TValue);
		return;
	}
	if (ullIndex == 0) { // and number of elements is not 0
		CNotList<T>::CNotListNode *pOldHead = pHead;
		pHead = pAllocateAndInitializeNewElement(TValue, pOldHead);
		++ullSize;
		return;
	}
	// inserting somewhere between the elements
	CNotList<T>::CNotListNode *pNextElement = pGetElementByIndex(ullIndex);
	// ullIndex > 0 is guaranteed
	CNotList<T>::CNotListNode *pPrevElement = pGetElementByIndex(ullIndex - 1);
	CNotList<T>::CNotListNode *pNewElement = pAllocateAndInitializeNewElement(
		TValue, pNextElement, pPrevElement
	);
	pPrevElement->pNext = pNewElement;
	pNextElement->pPrev = pNewElement;
	++ullSize;
}

template <typename T>
void CNotList<T>::vRemoveElement(const notSize_t &ullIndex) {
	vRemoveElementByPointer(pGetElementByIndex(ullIndex));
}

template <typename T>
typename CNotList<T>::CNotListNode *CNotList<T>::pGetElementByIndex(const notSize_t &ullIndex) const {
	CNotList<T>::CNotListNode *pCurrentNode = this->pHead;
	notSize_t ullCounter = 0;
	while (ullCounter != this->ullSize) {
		if (ullCounter == ullIndex) {
			return pCurrentNode;
		}
		pCurrentNode = pCurrentNode->pNext;
		++ullCounter;
	}
	throw "Bad element index.";
	return nullptr;
}

template <typename T>
typename CNotList<T>::CNotListNode *CNotList<T>::pAllocateNewElement() {
	CNotListNode *pNewElement = (CNotList<T>::CNotListNode *)malloc(sizeof(CNotList<T>::CNotListNode));
	if (pNewElement == NULL) throw "Bad memory allocation.";

	return pNewElement;
}

template <typename T>
void CNotList<T>::vRemoveElementByPointer(CNotListNode *pElementToRemove) {
	if (pElementToRemove->pPrev != nullptr) {
		pElementToRemove->pPrev->pNext = pElementToRemove->pNext;
		if (pElementToRemove == pTail) pTail = pElementToRemove->pPrev;
	}
	if (pElementToRemove->pNext != nullptr) {
		pElementToRemove->pNext->pPrev = pElementToRemove->pPrev;
		if (pElementToRemove == pHead) pHead = pElementToRemove->pNext;
	}

	free(pElementToRemove);
	--ullSize;

	if (ullSize == 1) pHead = pTail;
	if (ullSize == 0) {
		pHead = nullptr;
		pTail = nullptr;
	}
}

template <typename T>
typename CNotList<T>::CNotListNode *CNotList<T>::pAllocateAndInitializeNewElement(
	const T &TValue, CNotListNode *pNext, CNotListNode *pPrev) {
	CNotListNode *pNewElement = pAllocateNewElement();
	pNewElement->CNotList::CNotListNode::vSetMembers(TValue, pNext, pPrev);

	return pNewElement;
}

template <typename T>
void CNotList<T>::vReserve(const notSize_t &ullNumberOfElements) {
	for (notSize_t ullI = 0; ullI < ullNumberOfElements; ullI++) {
		this->vPush_back(0);
	}
}

template <typename T>
typename CNotList<T>::CNotListNode *CNotList<T>::pCopyNodeFromAnother(CNotListNode *pAnotherNode) {
	return pAllocateAndInitializeNewElement(pAnotherNode->m_value, pAnotherNode->pNext, pAnotherNode->pPrev);
}

template <typename T>
void CNotList<T>::CNotListNode::vSetMembers(const T &TValue, CNotListNode *pNext, CNotListNode *pPrev) {
	this->m_value = TValue;
	this->pNext = pNext;
	this->pPrev = pPrev;
}

template <typename T>
T CNotList<T>::operator[](const notSize_t &ullIndex) const {
	return pGetElementByIndex(ullIndex)->m_value;
}

template <typename T>
notSize_t CNotList<T>::ullGetSize() const {
	return ullSize;
}

template <typename T>
CNotList<T>::~CNotList() {
	if (ullSize == 0) return;
	CNotListNode *pCurrent = pHead->pNext;
	while (pCurrent != nullptr) {
		free(pCurrent->pPrev);
		pCurrent = pCurrent->pNext;
	}
	free(pTail);
	pHead = nullptr;
	pTail = nullptr;
	ullSize = 0;
}