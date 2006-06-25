// vim:tabstop=4:shiftwidth=4:noexpandtab:textwidth=80

/*
 * $RCSfile$
 *
 * $Log$
 * Revision 1.9  2006/06/25 09:27:19  hockm0bm
 * mem leak fixed
 *         - thanks to Jozo
 *
 * Revision 1.8  2006/06/22 18:47:27  hockm0bm
 * * deprecated functions replaced
 * * new test cases for ambiguous page tree
 *
 * Revision 1.7  2006/05/27 21:08:10  misuj1am
 *
 * -- tests improved
 * 	-- tests are testing every page instead of only on the first page
 *
 * Revision 1.6  2006/05/15 18:33:03  hockm0bm
 * test improvements
 *
 * Revision 1.5  2006/05/14 23:53:54  misuj1am
 *
 * -- test fixed
 *
 * Revision 1.4  2006/05/14 21:10:19  hockm0bm
 * content stream to xpdf Object test
 *         - doesn't work properly
 *
 * Revision 1.3  2006/05/13 22:19:29  hockm0bm
 * isInValidPdf refactored to hasValidPdf or isPdfValid functions
 *
 * Revision 1.2  2006/05/08 14:47:46  hockm0bm
 * * clone for FileStream test
 *         - seems to work
 * * clone for FileStream substream test
 *         - seems to work
 *
 * Revision 1.1  2006/05/06 21:16:22  hockm0bm
 * test class for streams
 *
 *
 */

#include <errno.h>
#include "testmain.h"
#include "../xpdf.h"

class TestStream: public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(TestStream);
		CPPUNIT_TEST(Test);
	CPPUNIT_TEST_SUITE_END();

public:

	bool compareStreams(Stream * str1, Stream * str2, int count=-1)
	{
		int ch1, ch2;
		str1->reset();
		str2->reset();
		do
		{
			// decrement only if count parameter is used
			if(count!=-1)
				count--;

			ch1=str1->getChar();
			ch2=str2->getChar();
			if(ch1!=ch2)
				return false;
			
			// if count bytes read, ends loop
			if(!count)
				break;

		}while(ch1!=EOF);

		return true;
	}
	
	void fileStreamTC(string fileName)
	{

		printf("%s fileName %s\n", __FUNCTION__, fileName.c_str());
		FILE * f1=fopen(fileName.c_str(), "r+");
		if(!f1)
		{
			printf("File open failed: %s\n", strerror(errno));
			return;
		}

		// creates unlimited stream from file
		Object dict;
		dict.initNull();
		FileStream * unlimitedStream=new FileStream(f1, 0, false, 0, &dict);

		printf("TC01:\tcontent of FileStream is same as file's content\n");
		// opens same file 
		FILE * f2=fopen(fileName.c_str(), "r+");
		int ch1, ch2;
		while((ch1=unlimitedStream->getChar())!=EOF)
		{
			ch2=fgetc(f2);
			CPPUNIT_ASSERT(ch1==ch2);
		}
		fclose(f2);

		printf("TC02:\tcloned stream's content is same as original\n");
		Stream * clonedStream=unlimitedStream->clone();
		CPPUNIT_ASSERT(compareStreams(unlimitedStream, clonedStream));

		printf("TC03:\tsubstream clone test\n");
		Stream * subStream=unlimitedStream->makeSubStream(0, true, 1, &dict);
		Stream * cloneSubStream=subStream->clone();
		CPPUNIT_ASSERT(compareStreams(subStream, cloneSubStream, 1));
		
		delete cloneSubStream;
		delete subStream;
		delete unlimitedStream;
		delete clonedStream;
		fclose(f1);
	}

	void contentStreamTC(CPdf & pdf)
	{
	using namespace boost;
	using namespace std;
	using namespace pdfobjects;
	using namespace pdfobjects::utils;

		printf("%s\n", __FUNCTION__);

		printf("%u pages found\n", pdf.getPageCount());
		for(size_t i=1; i<=pdf.getPageCount(); i++)
		{
			// gets page dictionary at position and gets Contents 
			// property from it
			shared_ptr<CDict> pageDict=pdf.getPage(i)->getDictionary();
			printf("Page #%d\n", i);
			try
			{
				shared_ptr<IProperty> contentProp=pageDict->getProperty("Contents");
				vector<IndiRef> streamRefs;
				if(! isRef(*contentProp) && !isArray(*contentProp))
				{
					printf("\tPage %u has uncorect Contents entry type=%d\n", i, contentProp->getType());
					continue;
				}else
				{
					if(isRef(*contentProp))
						streamRefs.push_back(getValueFromSimple<CRef>(contentProp));
					else
					{
						// gets CArray
						shared_ptr<CArray> contentArray=IProperty::getSmartCObjectPtr<CArray>(contentProp);
						// collects all referencies
						for(size_t i=0; i<contentArray->getPropertyCount(); i++)
						{
							shared_ptr<IProperty> element=contentArray->getProperty(i);
							if(!isRef(element))
								// just silently ignores non valid members
								continue;
							streamRefs.push_back(getValueFromSimple<CRef>(element));
						}
					}
				}
				
				// checks all content streams
				int index=1;
				for(vector<IndiRef>::iterator i=streamRefs.begin(); i!=streamRefs.end(); i++, index++)
				{
					printf("\tStream number %d\n", index);
					IndiRef contentRef=*i;
					shared_ptr<CStream> contentStr=IProperty::getSmartCObjectPtr<CStream>(pdf.getIndirectProperty(contentRef));

					int ch1, ch2;
					size_t bytes=0;

					// creates xpdf Object represenation and checks it
					Object * xpdfContentStr=contentStr->_makeXpdfObject();
					xpdfContentStr->getStream()->reset();
					const CStream::Buffer & buffer=contentStr->getBuffer();
					// xpdf content must be same as in CStream object
					BaseStream * baseStream=xpdfContentStr->getStream()->getBaseStream();
					baseStream->reset();

					printf("TC01:\tCStream::_makeXpdfObject object is same as original\n");
					while((ch1=baseStream->getChar())!=EOF)
					{
						CPPUNIT_ASSERT((unsigned char)ch1==(unsigned char)buffer[bytes]);
						bytes++;
					}

					// total number of bytes must be correct
					printf("TC02:\tall bytes read test\n");
					Object streamLen;
					// FIXME
					//CPPUNIT_ASSERT(bytes==contentStr->getLength());

					// checks cloning of content stream
					Object * xpdfContentClone=xpdfContentStr->clone();
					if(xpdfContentClone)
					{
						printf("TC03:\tcloned content stream is same as original test\n");
						CPPUNIT_ASSERT(compareStreams(xpdfContentClone->getStream(), xpdfContentStr->getStream()));

						printf("TC04:\tcloned content base stream is same as original test\n");
						BaseStream * cloneBaseStream=xpdfContentClone->getStream()->getBaseStream();
						CPPUNIT_ASSERT(compareStreams(cloneBaseStream, baseStream));

						freeXpdfObject(xpdfContentClone);
					}else
						printf("\t\tstream cloning failed. Stream kind is %d\n", xpdfContentClone->getStream()->getKind());
						
					// directly fetched stream must be same as created
					Object fetchedContentStr;
					pdf.getCXref()->fetch(contentRef.num, contentRef.gen, &fetchedContentStr);
					printf("TC05:\tfetched content stream stream is same as original\n");
					CPPUNIT_ASSERT(compareStreams(fetchedContentStr.getStream(), xpdfContentStr->getStream()));
					BaseStream * baseStreamFetched=fetchedContentStr.getStream()->getBaseStream();
					printf("TC06:\tfetched content base stream stream is same as original\n");
					CPPUNIT_ASSERT(compareStreams(baseStream, baseStreamFetched));
					
					// deallocates all objects
					freeXpdfObject(xpdfContentStr);
					fetchedContentStr.free();
				}
			}catch(ElementNotFoundException & e)
			{
				printf("\t\tPage %u has no content stream\n", i);
			}
		}
	}
	
	virtual ~TestStream()
	{
	}

	void setUp()
	{
	}


	void tearDown()
	{
	}

	void Test()
	{
		for(FileList::iterator i=fileList.begin(); i!=fileList.end(); i++)
		{
			fileStreamTC(*i);
			CPdf * pdf=CPdf::getInstance((*i).c_str(), CPdf::ReadOnly);
			//contentStreamTC(*pdf);
			pdf->close();
		}
	}
};
CPPUNIT_TEST_SUITE_REGISTRATION(TestStream);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestStream, "TEST_STREAM");
