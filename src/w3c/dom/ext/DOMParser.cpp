#include <assert.h>
#include <string.h>
#include <expat.h>
#if SVGL_USE_GC
#include <gc.h>
#endif
#include <map>
#include <vector>
#include <stack>
#include <utils/InputSource.hpp>
#include <utils/FileInputSource.hpp>
#include <utils/EntityResolver.hpp>
#include <w3c/dom/DOMImplementation.hpp>
#include <w3c/dom/Document.hpp>
#include <w3c/dom/AttrDef.hpp>
#include <w3c/dom/ext/DOMParser.hpp>
#include <w3c/dom/ext/ErrorHandler.hpp>
#include <w3c/dom/dom.hpp>

namespace dom {

#define NS_SEP '\001'

#if SVGL_USE_GC
typedef std::stack<String*,std::vector<String*, gc_alloc<String*> > > StringStack;
#else
typedef std::stack<String*,std::vector<String*> > StringStack;
#endif

#if SVGL_USE_GC
typedef std::map<String*,
		 StringStack,
		 string_less,
		 gc_alloc<std::pair<const String*,StringStack> > >
NSMap;
#else
typedef std::map<String*,
		 StringStack,
		 string_less
		 >
NSMap;
#endif


struct ParseContext {
  XML_Parser parser;
  DOMParser& domparser;
  Document * document;
  Node * current;
  String * URL;
  StringStack defaultNS;
  NSMap nsMap;
  bool inCDATASection;

  ParseContext(DOMParser& p, XML_Parser pa, String * url)
    : parser(pa),
      domparser(p),
      document(null),
      current(null),
      URL(url),
      inCDATASection(false)
  {
  }

  Document * getDocument(String * elname) {
    if (document == null) {
      DOMImplementation * impl = DOMImplementation::getDOMImplementation();
      document = impl->createDocument(null,
				      elname,
				      null);
      current = document;
    }
    return document;
  }
  Document * createDocument(String * namespaceURI, 
			    String * qualifiedName, 
			    DocumentType * doctype)
  {
    DOMImplementation * impl = DOMImplementation::getDOMImplementation();
    document = impl->createDocument(namespaceURI,
				    qualifiedName,
				    doctype);

    current = document;
    return document;
  }

  void pushNS(String * prefix, String * uri) {
    if (prefix != null) {
      NSMap::iterator i = nsMap.find(prefix);
      if (i == nsMap.end()) {
	std::pair<NSMap::iterator,bool> p;
	p = nsMap.insert(NSMap::value_type(prefix,StringStack()));
	i = p.first;
      }
      i->second.push(uri);
    }
    else
      defaultNS.push(uri);
  }
  void popNS(String * prefix) {
    if (prefix != null) {
      NSMap::iterator i = nsMap.find(prefix);
      if (i == nsMap.end()) {
	return;			// should not occur
      }
      i->second.pop();
    }
    else
      defaultNS.pop();
  }
  String * getNamespaceURI() {
    if (defaultNS.size() != 0)
      return defaultNS.top();
    else
      return null;
  }
};

DOMParser::DOMParser()
  : document(0),
    currentNode(0),
    inDocument(false),
    inCDATASection(false),
    attrName(null),
    entityResolver(sdom::EntityResolver::instance()),
    errorHandler(null)
{
}

DOMParser::~DOMParser()
{
}

void
DOMParser::reset()
{
  document = 0;
  currentNode= 0;
  inDocument = false;
  inCDATASection = false;
  attrName = null;
}

void
DOMParser::parse(String * systemId)
{
  sdom::FileInputSource * input = new sdom::FileInputSource(systemId);
  if (input->finished())
    throw DOMException(DOMException::NOT_FOUND_ERR);
  parse(input);
  //delete input;
}

static int ExternalEntityRefHandler(XML_Parser parser,
				    const XML_Char *context,
				    const XML_Char *base,
				    const XML_Char *systemId,
				    const XML_Char *publicId);

static void ElementDeclHandler(void *userData,
				     const XML_Char *name,
				     XML_Content *model);


static void AttlistDeclHandler(void *userData,
				     const XML_Char	*elname,
				     const XML_Char	*attname,
				     const XML_Char	*att_type,
				     const XML_Char	*dflt,
				     int		isrequired);

static void StartElementHandler(void *userData,
			 const XML_Char *name,
			 const XML_Char **atts);

static void EndElementHandler(void *userData,
			      const XML_Char *name);

/* s is not 0 terminated. */
static void CharacterDataHandler(void *userData,
				 const XML_Char *s,
				 int len);
/* target and data are 0 terminated */
static void ProcessingInstructionHandler(void *userData,
					 const XML_Char *target,
					 const XML_Char *data);

/* data is 0 terminated */
static void CommentHandler(void *userData, const XML_Char *data);

static void StartCdataSectionHandler(void *userData);
static void EndCdataSectionHandler(void *userData);

static void StartDoctypeDeclHandler(void *userData,
				    const XML_Char *doctypeName,
				    const XML_Char *sysid,
				    const XML_Char *pubid,
				    int has_internal_subset
				    );

static void EndDoctypeDeclHandler(void *userData);

static void StartNamespaceDeclHandler(void *userData,
				      const XML_Char *prefix,
				      const XML_Char *uri);

static void EndNamespaceDeclHandler(void *userData,
				    const XML_Char *prefix);

#if SVGL_USE_GC
XML_Memory_Handling_Suite gc_malloc_suite = {
  GC_malloc_uncollectable,
  GC_realloc,
  GC_free
};
#else
XML_Memory_Handling_Suite gc_malloc_suite = {
  malloc,
  realloc,
  free
};
#endif

void
DOMParser::parse(sdom::InputSource * input)
{
  const XML_Char ns_sep(NS_SEP);

  XML_Parser parser = XML_ParserCreate_MM(0,
					  &gc_malloc_suite,
					  //0,
					  &ns_sep);
  XML_SetReturnNSTriplet(parser, 1); // returns uri, prefix and localname
  ParseContext ctx(*this, parser, input->getURI());
  XML_SetUserData(parser, &ctx);
  XML_SetParamEntityParsing	(parser,
				 XML_PARAM_ENTITY_PARSING_ALWAYS);
  XML_SetExternalEntityRefHandler(parser,
				  ExternalEntityRefHandler);
  XML_SetElementDeclHandler	(parser,
				 ElementDeclHandler);
  XML_SetAttlistDeclHandler	(parser,
				 AttlistDeclHandler);
  XML_SetElementHandler		(parser,
				 StartElementHandler,
				 EndElementHandler);
  XML_SetCharacterDataHandler	(parser, CharacterDataHandler);
  XML_SetProcessingInstructionHandler(parser, ProcessingInstructionHandler);
  XML_SetCommentHandler		(parser, CommentHandler);
  XML_SetCdataSectionHandler	(parser,
				 StartCdataSectionHandler,
				 EndCdataSectionHandler);
  XML_SetDoctypeDeclHandler	(parser,
				 StartDoctypeDeclHandler,
				 EndDoctypeDeclHandler);
  XML_SetNamespaceDeclHandler(parser,
			      StartNamespaceDeclHandler,
			      EndNamespaceDeclHandler);
  
  try {
    for (;;) {
      int bytes_read;
#define BUFF_SIZE 1024
      void * buffer = XML_GetBuffer(parser, BUFF_SIZE);
      if (buffer == 0)
	throw DOMException(DOMException::DOMSTRING_SIZE_ERR);
      bytes_read = input->readBytes((char*)buffer, BUFF_SIZE);
      if (bytes_read  < 0)
	throw DOMException(DOMException::DOMSTRING_SIZE_ERR);
      if (XML_ParseBuffer(parser, bytes_read, bytes_read == 0) == 0) {
				std::cerr << "line:" << XML_GetCurrentLineNumber(parser) << " column:" << XML_GetCurrentColumnNumber(parser) << " " <<  XML_ErrorString(XML_GetErrorCode(parser)) << " " __FILE__ ":" << __LINE__ << std::endl;
				throw DOMException(DOMException::SYNTAX_ERR);
			}
      if (bytes_read == 0)
	break;
    }
  }
  catch(...) {
    XML_ParserFree(parser);
    throw;
  }
  XML_ParserFree(parser);
  document = ctx.document;
  
}

void
DOMParser::setEntityResolver(sdom::EntityResolver * resolver)
{
  entityResolver = resolver;
}

sdom::EntityResolver *
DOMParser::getEntityResolver() const
{
  return entityResolver;
}

void
DOMParser::setErrorHandler(ErrorHandler * eh)
{
  errorHandler = eh;
}

ErrorHandler *
DOMParser::getErrorHandler() const
{
  return errorHandler;
}

static String *
qualifiedString(const XML_Char * str)
{
  char * sep = strchr(str, NS_SEP);
  String * eltName;

  if (sep != null) {		// create the prefixed name
    XML_Char buffer[1024];
    strcpy(buffer, sep+1);
    strcat(buffer, ":");
    strncat(buffer, str, sep - str);
    eltName = String::internStringUtf8(buffer);
  }
  else
    eltName = String::internStringUtf8(str);
  return eltName;
}

static void StartElementHandler(void *userData,
				const XML_Char *name,
				const XML_Char **atts)
{
  ParseContext * ctx = reinterpret_cast<ParseContext*>(userData);
  //std::cout << "startElement " << name << std::endl;

  String * namespaceURI = ctx->getNamespaceURI();
  String * eltName;
  char * colon = strchr(name, NS_SEP);

  if (colon != null) {
    namespaceURI = String::internStringUtf8(name, colon - name);
    eltName = qualifiedString(colon+1);
  }
  else {
    eltName = String::internStringUtf8(name);
  }

#if 0
  DocumentType * doctype = ctx->getDocument(eltName)->getDoctype();
  if (ctx->current == ctx->document &&
      ctx->document->getDocumentElement() == null &&
      doctype == null) {
    DOMImplementation * impl = DOMImplementation::getDOMImplementation();
    doctype = impl->createDocumentType(eltName, null, null);
    if (doctype != null)
      ctx->document->appendChild(doctype);
  }
#endif
  Element * e = ctx->document->createElementNS(namespaceURI,
					       eltName);
  ctx->current->appendChild(e);
  ctx->current = e;
  int attCnt = XML_GetSpecifiedAttributeCount(ctx->parser);
  for (const XML_Char ** a = atts; *a != 0; a += 2, attCnt -= 2) {
    colon = strchr(a[0], NS_SEP);
    String * attrName;
    if (colon == null) {
      namespaceURI = ctx->getNamespaceURI();
      attrName = String::internStringUtf8(a[0]);
    }
    else {
      namespaceURI = String::internStringUtf8(a[0],colon - a[0]);
      attrName = qualifiedString(colon+1);
    }
    Attr * attr = e->getAttributeNodeNS(namespaceURI, attrName);
    if (attr == null) {
      attr = ctx->document->createAttributeNS(namespaceURI, attrName);
      e->setAttributeNodeNS(attr);
    }
    String * attrValue = String::createStringUtf8(a[1]); 
    attr->setNodeValue(attrValue);
    attr->setSpecified(attCnt > 0);
    {
      static unicode::String* id_string = unicode::String::internString("id");
      if(attrName==id_string) {
	ctx->document->putIdentifier(attrValue, e);
      }
    }

  }
}

static void EndElementHandler(void *userData,
			      const XML_Char *name)
{
  ParseContext * ctx = reinterpret_cast<ParseContext*>(userData);
  //std::cout << "endElement " << name << std::endl;

  ctx->current = ctx->current->getParentNode();
  assert(ctx->current != 0);
}

/* s is not 0 terminated. */
static void CharacterDataHandler(void *userData,
				 const XML_Char *s,
				 int len)
{
  ParseContext * ctx = reinterpret_cast<ParseContext*>(userData);
  //std::cout << "characterData " << std::endl;
  assert(ctx->document != 0 && ctx->current != 0);

  ctx->current->appendChild(ctx->document->createTextNode(String::createStringUtf8(s, len)));
}
/* target and data are 0 terminated */
static void ProcessingInstructionHandler(void *userData,
					 const XML_Char *target,
					 const XML_Char *data)
{
  ParseContext * ctx = reinterpret_cast<ParseContext*>(userData);
  //std::cout << "ProcessingInstruction " << std::endl;
  ctx->current
    ->appendChild(ctx->document->
		  createProcessingInstruction(String::internStringUtf8(target),
					      String::internStringUtf8(data)));
}

/* data is 0 terminated */
static void CommentHandler(void *userData, const XML_Char *data)
{
  ParseContext * ctx = reinterpret_cast<ParseContext*>(userData);
  //std::cout << "Comment " << std::endl;
  //assert(ctx->document != 0 && ctx->current != 0);
  
  if(ctx->document != 0 && ctx->current != 0) {
    ctx->current
      ->appendChild(ctx->document->createComment(String::createStringUtf8(data)));
  }
  else {
    //std::cerr << "comment while no document has been created (DOCTYPE not reached) " << __FILE__<< ":" << __LINE__ << std::endl;
  }
}

static void StartCdataSectionHandler(void *userData)
{
  ParseContext * ctx = reinterpret_cast<ParseContext*>(userData);
  ctx->inCDATASection = true;
}

static void EndCdataSectionHandler(void *userData)
{
  ParseContext * ctx = reinterpret_cast<ParseContext*>(userData);
  ctx->inCDATASection = false;
}

static void StartDoctypeDeclHandler(void *userData,
				    const XML_Char *doctypeName,
				    const XML_Char *sysid,
				    const XML_Char *pubid,
				    int has_internal_subset
				    )
{
  ParseContext * ctx = reinterpret_cast<ParseContext*>(userData);
  DOMImplementation * impl = DOMImplementation::getDOMImplementation();
  String * dtname = String::internStringUtf8(doctypeName);

  //std::cerr << doctypeName << " "__FILE__ ":" << __LINE__ << std::endl;
#if 1
  DocumentType * doctype =
    impl->createDocumentType(dtname,
			     String::internStringUtf8(pubid),
			     String::createStringUtf8(sysid));
  //std::cerr << (int*) doctype << " " __FILE__ ":" << __LINE__ << std::endl;
  ctx->createDocument(null, dtname, doctype);
#else
  DocumentType * doctype =
    impl->createDocumentType(dtname);
  //std::cerr << (int*) doctype << " " __FILE__ ":" << __LINE__ << std::endl;
  ctx->createDocument(null, dtname, doctype);
#endif
  ctx->current = doctype;
}

static void EndDoctypeDeclHandler(void *userData)
{
  ParseContext * ctx = reinterpret_cast<ParseContext*>(userData);

  ctx->current = ctx->document;
}

static int ExternalEntityRefHandler(XML_Parser parser,
				    const XML_Char *context,
				    const XML_Char *base,
				    const XML_Char *systemId,
				    const XML_Char *publicId)
{
  ParseContext * ctx =
    reinterpret_cast<ParseContext*>(XML_GetUserData(parser));

#if 0
  // avoid reloading DTD.  We'll see later
  DocumentType * doctype = ctx->document->getDoctypeImpl();
  if (doctype != null &&
      doctype->getSystemId()->equals(systemId) &&
      doctype->isLoaded())
    return 1;
#endif

  char buffer[BUFF_SIZE];
  String * pubId = String::internStringUtf8(publicId);
  String * sysId = String::internStringUtf8(systemId);
  sdom::EntityResolver * entityResolver = ctx->domparser.getEntityResolver();
  ErrorHandler * errorHandler = ctx->domparser.getErrorHandler();
  
  sdom::InputSource * input = entityResolver->resolveEntity(pubId, sysId);
  if (input == null) {
    ctx->URL->transcode(buffer, BUFF_SIZE);
    char *p = strrchr(buffer, '/');
    if (p != 0)
      p[1] = 0;
    if (base != null)
      strcat(buffer, base);
    strcat(buffer, systemId);
    sysId = String::createString(buffer);
    input = entityResolver->resolveEntity(pubId, sysId);
  }
  if (input == null)
    return 1;
  
  XML_Parser subparser = XML_ExternalEntityParserCreate(parser,
							context,
							NULL);
  if (subparser == 0) {
    //delete input;
    return 0;
  }
  int retcode = 1;
  try {
    for(;;) {
      int bytes_read;
      void *buff = XML_GetBuffer(subparser, BUFF_SIZE);
      if (buff == NULL) {
	retcode = 0;
	errorHandler->fatalError(DOMException(DOMException::NOT_SUPPORTED_ERR));
	XML_ParserFree(subparser);
	break;
      }

      bytes_read = input->readBytes((char *)buff, BUFF_SIZE);
      if (bytes_read < 0) {
	retcode = 0;
	errorHandler->fatalError(DOMException(DOMException::NOT_SUPPORTED_ERR));
	XML_ParserFree(subparser);
	break;
      }

      if (! XML_ParseBuffer(subparser, bytes_read, bytes_read == 0)) {
	retcode = 0;
	errorHandler->error(DOMException(DOMException::SYNTAX_ERR));
	XML_ParserFree(subparser);
	break;
      }
      if (bytes_read == 0)
	break;
    }
    XML_ParserFree(subparser);
  }
  catch(...) {
    XML_ParserFree(subparser);
    retcode = 0;
    //delete input;
    throw;
  }
  //delete input;
  return retcode;
}

static void ElementDeclHandler(void *userData,
			       const XML_Char *name,
			       XML_Content *model)
{
  ParseContext * ctx = reinterpret_cast<ParseContext*>(userData);
  //DocumentType * doctype = ctx->document->getDoctype();
  String * namespaceURI = null;
  String * localName;

  const XML_Char * colon = strchr(name, NS_SEP);
  if (colon == null)
    localName = String::internStringUtf8(name);
  else {
    namespaceURI = String::internStringUtf8(name, colon - name);
    localName = qualifiedString(colon+1);
  }

  Element * e = ctx->document->createElementNS(namespaceURI, localName);

  if (ctx->current->getNodeType() == Node::ELEMENT_NODE)
    ctx->current = ctx->current->getParentNode();
  assert(ctx->current->getNodeType() == Node::DOCUMENT_TYPE_NODE);
  ctx->current->appendChild(e);
  ctx->current = e;
}


static AttrDef::AttributeType
attribute_type(const XML_Char * att_type)
{
#define CHECK_TYPE(TYPE) \
  if (strcmp(att_type, #TYPE) == 0) \
    return AttrDef::TYPE##_ATTR_TYPE;

  CHECK_TYPE(CDATA);
  CHECK_TYPE(ID);
  CHECK_TYPE(IDREF);
  CHECK_TYPE(IDREFS);
  CHECK_TYPE(NMTOKEN);
  CHECK_TYPE(NMTOKENS);
  CHECK_TYPE(ENTITY_);
  CHECK_TYPE(ENTITIES);
#undef CHECK_TYPE
  if (strncmp(att_type, "NOTATION", 8) == 0)
    return AttrDef::NOTATION_ATTR_TYPE;
  if (att_type[0] == '(')
    return AttrDef::ENUM_ATTR_TYPE;
  return AttrDef::CDATA_ATTR_TYPE;	// error !
}

static void
create_enums(StringList& sl,
	     const XML_Char * att_type)
{
  const XML_Char * beg;
  const XML_Char * end;
  assert(*att_type == '(');
  beg = att_type+1;
  end = beg+1;
  for(;;) {
    while (*end != '|' &&
	   *end != ')' &&
	   *end !=' ' &&
	   *end != 0)
      end++;
    sl.push_back(String::internStringUtf8(beg, end - beg));
    if (*end == ')' ||
	*end == 0)
      return;
    beg = end = end+1;
  }
}

static void
AttlistDeclHandler(void *userData,
		   const XML_Char	*elname,
		   const XML_Char	*attname,
		   const XML_Char	*att_type,
		   const XML_Char	*dflt,
		   int		isrequired)
{
//   if (dflt == null &&
//       isrequired == 0)
//     return;
  ParseContext * ctx = reinterpret_cast<ParseContext*>(userData);
  //assert(ctx->current->getNodeType() == Node::ELEMENT_NODE);
  String * namespaceURI = null;
  String * localName;

  if (attname[0] == 'x'
      && attname[1] == 'm'
      && attname[2] == 'l'
      && attname[3] == 'n'
      && attname[4] == 's'
      && (attname[5] == '\0' || attname[5] == ':')) {
    return;			// ignore namespaces
  }
  localName = String::internStringUtf8(attname);

  AttrDef::AttributeType t = attribute_type(att_type);
  StringList sl;

      
  if (t == AttrDef::ENUM_ATTR_TYPE)
    create_enums(sl, att_type);
  else if (t == AttrDef::NOTATION_ATTR_TYPE)
    create_enums(sl, att_type+8);
  AttrDef * attr = new AttrDef(ctx->document,
				       namespaceURI,
				       localName,
				       t, isrequired != 0, sl);
  if (dflt != null)
    attr->setValue(String::internStringUtf8(dflt));
  ctx->current->getAttributes()->setNamedItem(attr);
}

static void StartNamespaceDeclHandler(void *userData,
				      const XML_Char *prefix,
				      const XML_Char *uri)
{
  ParseContext * ctx = reinterpret_cast<ParseContext*>(userData);
  ctx->pushNS(String::internStringUtf8(prefix),
	      String::internStringUtf8(uri));
}

static void EndNamespaceDeclHandler(void *userData,
				    const XML_Char *prefix)
{
  ParseContext * ctx = reinterpret_cast<ParseContext*>(userData);
  ctx->popNS(String::internStringUtf8(prefix));
}

}

