#pragma once


#include <exception>
#include <search.h>
#include <vector>



#define XML_STATIC
#include "..\..\Libraries\expat\lib\expat.h"

namespace IW
{
	typedef unsigned char int8u;

	namespace svg
	{



		class path_tokenizer
		{
		public:
			path_tokenizer()
				: m_path(0), m_last_command(0), m_last_number(0.0)
			{
				//------------------------------------------------------------------------
				const char s_commands[]   = "+-MmZzLlHhVvCcSsQqTtAaFfPp";
				const char s_numeric[]    = ".Ee0123456789";
				const char s_separators[] = " ,\t\n\r";


				init_char_mask(m_commands_mask,   s_commands);
				init_char_mask(m_numeric_mask,    s_numeric);
				init_char_mask(m_separators_mask, s_separators);
			}

			void set_path_str(const char* str)
			{
				m_path = str;
				m_last_command = 0;
				m_last_number = 0.0;
			}

			bool next()
			{
				if(m_path == 0) return false;

				// Skip all white spaces and other garbage
				while(*m_path && !is_command(*m_path) && !is_numeric(*m_path)) 
				{
					if(!is_separator(*m_path))
					{
						char buf[100];
						sprintf(buf, "path_tokenizer::next : Invalid Character %c", *m_path);
						throw exception(buf);
					}
					m_path++;
				}

				if(*m_path == 0) return false;

				if(is_command(*m_path))
				{
					// Check if the command is a numeric sign character
					if(*m_path == '-' || *m_path == '+')
					{
						return parse_number();
					}
					m_last_command = *m_path++;
					while(*m_path && is_separator(*m_path)) m_path++;
					if(*m_path == 0) return true;
				}
				return parse_number();
			}

			Gdiplus::REAL next(char cmd)
			{
				if(!next()) throw exception("parse_path: Unexpected end of path");
				if(last_command() != cmd)
				{
					char buf[100];
					sprintf(buf, "parse_path: Command %c: bad or missing parameters", cmd);
					throw exception(buf);
				}
				return last_number();
			}

			char   last_command() const { return m_last_command; }
			Gdiplus::REAL last_number() const { return m_last_number; }


		private:
			static void init_char_mask(char* mask, const char* char_set)
			{
				memset(mask, 0, 256/8);
				while(*char_set) 
				{
					unsigned c = unsigned(*char_set++) & 0xFF;
					mask[c >> 3] |= 1 << (c & 7);
				}
			}

			bool contains(const char* mask, unsigned c) const
			{
				return (mask[(c >> 3) & (256/8-1)] & (1 << (c & 7))) != 0;
			}

			bool is_command(unsigned c) const
			{
				return contains(m_commands_mask, c);
			}

			bool is_numeric(unsigned c) const
			{
				return contains(m_numeric_mask, c);
			}

			bool is_separator(unsigned c) const
			{
				return contains(m_separators_mask, c);
			}

			bool parse_number()
			{
				char buf[256]; // Should be enough for any number
				char* buf_ptr = buf;

				// Copy all sign characters
				while(buf_ptr < buf+255 && *m_path == '-' || *m_path == '+')
				{
					*buf_ptr++ = *m_path++;
				}

				// Copy all numeric characters
				while(buf_ptr < buf+255 && is_numeric(*m_path))
				{
					*buf_ptr++ = *m_path++;
				}
				*buf_ptr = 0;
				m_last_number = static_cast<float>(atof(buf));
				return true;
			}

			char m_separators_mask[256/8];
			char m_commands_mask[256/8];
			char m_numeric_mask[256/8];

			const char* m_path;
			Gdiplus::REAL m_last_number;
			char   m_last_command;
		};

		//============================================================================
		// Basic path attributes
		struct path_attributes
		{
			Gdiplus::Color            fill_color;
			Gdiplus::Color            stroke_color;
			bool                      fill_flag;
			bool                      stroke_flag;
			bool                      even_odd_flag;
			Gdiplus::LineJoin line_join;
			Gdiplus::LineCap  line_cap;
			double                    miter_limit;
			double                    stroke_width;
			Gdiplus::Matrix              *pTransform;


			// Empty constructor
			path_attributes() :
			fill_color(Gdiplus::Color(0,0,0,0)),
				stroke_color(Gdiplus::Color(0,0,0,0)),
				fill_flag(true),
				stroke_flag(false),
				even_odd_flag(false),
				line_join(Gdiplus::LineJoinMiter),
				line_cap(Gdiplus::LineCapFlat),
				miter_limit(4.0),
				stroke_width(1.0),		
				pTransform(new Gdiplus::Matrix())
			{
			}

			// Copy constructor
			path_attributes(const path_attributes& attr) :
			fill_color(attr.fill_color),
				stroke_color(attr.stroke_color),
				fill_flag(attr.fill_flag),
				stroke_flag(attr.stroke_flag),
				even_odd_flag(attr.even_odd_flag),
				line_join(attr.line_join),
				line_cap(attr.line_cap),
				miter_limit(attr.miter_limit),
				stroke_width(attr.stroke_width),
				pTransform(attr.pTransform->Clone())
			{	
			}

			void operator=(const path_attributes& attr)
			{
				fill_color = attr.fill_color;
				stroke_color = attr.stroke_color;
				fill_flag = attr.fill_flag;
				stroke_flag = attr.stroke_flag;
				even_odd_flag = attr.even_odd_flag;
				line_join = attr.line_join;
				line_cap = attr.line_cap;
				miter_limit = attr.miter_limit;
				stroke_width = attr.stroke_width;
				pTransform = attr.pTransform->Clone();
			}

			~path_attributes()
			{
				delete pTransform;
			}


		};


		struct path
		{
			Gdiplus::GraphicsPath *pPath;
			Gdiplus::PointF point;
			path_attributes attr;

			~path()
			{
				delete pPath;
			}

			// Empty constructor
			path() :
				point(0.0f, 0.0f),	
				pPath(new Gdiplus::GraphicsPath)
			{
			}

			// Copy constructor
			path(const path& in) :
				point(in.point),				
				pPath(in.pPath->Clone()),
				attr(in.attr)
			{	
			}

			void operator=(const path& in)
			{
				point = in.point;
				pPath = in.pPath->Clone();
				attr = in.attr;
			}

			void MoveTo(Gdiplus::REAL x, Gdiplus::REAL y, bool rel=false)
			{
				if (rel)
				{
					x += point.X; 
					y += point.Y;
				}

				point.X = x;
				point.Y = x;
			}

			void LineTo(Gdiplus::REAL x, Gdiplus::REAL y, bool rel=false)
			{
				if (rel)
				{
					x += point.X; 
					y += point.Y;
				}

				pPath->AddLine(point.X, point.Y, x, y);
				point.X = x;
				point.Y = y;
			}

			void CurveTo4(Gdiplus::REAL x1, Gdiplus::REAL y1, Gdiplus::REAL x2, Gdiplus::REAL y2, Gdiplus::REAL x3, Gdiplus::REAL y3, bool rel=false)
			{
				if (rel)
				{
					x1 += point.X; 
					y1 += point.Y;
					x2 += point.X; 
					y2 += point.Y;
					x3 += point.X; 
					y3 += point.Y;
				}

				pPath->AddBezier(point.X, point.Y, x1, y1, x2, y2, x3, y3);
				point.X = x3;
				point.Y = y3;
			}

			void CurveTo4(Gdiplus::REAL x1, Gdiplus::REAL y1, Gdiplus::REAL x2, Gdiplus::REAL y2, bool rel=false)
			{
				CurveTo4(x1, y1, x2, y2, x2, y2, rel);
			}

			void CurveTo3(Gdiplus::REAL x1, Gdiplus::REAL y1, Gdiplus::REAL x2, Gdiplus::REAL y2, bool rel=false)
			{
				CurveTo4(x1, y1, x2, y2, x2, y2, rel);
			}

			void CurveTo3(Gdiplus::REAL x, Gdiplus::REAL y, bool rel=false)
			{
				CurveTo4(x, y, x, y, x, y, rel);
			}

		};


		class path_renderer
		{
		public:

			std::vector<path_attributes> m_attr_storage;
			std::vector<path> m_path_storage;

			void Render(Gdiplus::Graphics &graphics, CRect &rect)
			{
				graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
				graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);

				graphics.Clear(Gdiplus::Color(0xFFC0E337));

				Gdiplus::Rect rectPath;

				for(unsigned i = 0; i < m_svg.m_path_storage.size(); i++)
				{
					Gdiplus::Rect rect;
					m_svg.m_path_storage[i].pPath->GetBounds(&rect, NULL, NULL);	
					Gdiplus::Rect::Union(rectPath, rectPath, rect);
				}

				Gdiplus::REAL x = (rect.Width() / 2.0f) - (rectPath.X + (rectPath.Width / 2.0f));
				Gdiplus::REAL y = (rect.Height() / 2.0f) - (rectPath.Y + (rectPath.Height / 2.0f));

				Gdiplus::Matrix m;
				m.Translate(x, y);
				graphics.SetTransform(&m);

				for(unsigned i = 0; i < m_svg.m_path_storage.size(); i++)
				{
					graphics.FillPath(&m_brush, m_svg.m_path_storage[i].pPath);
				}				
			}

			void remove_all()
			{
				m_attr_storage.clear();
				m_path_storage.clear();
			}

			path_attributes &cur_attr()
			{
				return *(m_attr_storage.rbegin());
			}

			path &cur_path()
			{
				return *(m_path_storage.rbegin());
			}

			// Use these functions as follows:
			// begin_path() when the XML tag <path> comes ("start_element" handler)
			// parse_path() on "d=" tag attribute
			// end_path() when parsing of the entire tag is done.
			void begin_path()
			{
				m_path_storage.push_back(path());		
			}

			void parse_path(path_tokenizer& tok)
			{
				while(tok.next())
				{
					Gdiplus::REAL arg[10];
					char cmd = tok.last_command();
					unsigned i;
					switch(cmd)
					{
					case 'M': case 'm':
						arg[0] = tok.last_number();
						arg[1] = tok.next(cmd);
						move_to(arg[0], arg[1], cmd == 'm');
						break;

					case 'L': case 'l':
						arg[0] = tok.last_number();
						arg[1] = tok.next(cmd);
						line_to(arg[0], arg[1], cmd == 'l');
						break;

					case 'V': case 'v':
						vline_to(tok.last_number(), cmd == 'v');
						break;

					case 'H': case 'h':
						hline_to(tok.last_number(), cmd == 'h');
						break;

					case 'Q': case 'q':
						arg[0] = tok.last_number();
						for(i = 1; i < 4; i++)
						{
							arg[i] = tok.next(cmd);
						}
						curve3(arg[0], arg[1], arg[2], arg[3], cmd == 'q');
						break;

					case 'T': case 't':
						arg[0] = tok.last_number();
						arg[1] = tok.next(cmd);
						curve3(arg[0], arg[1], cmd == 't');
						break;

					case 'C': case 'c':
						arg[0] = tok.last_number();
						for(i = 1; i < 6; i++)
						{
							arg[i] = tok.next(cmd);
						}
						curve4(arg[0], arg[1], arg[2], arg[3], arg[4], arg[5], cmd == 'c');
						break;

					case 'S': case 's':
						arg[0] = tok.last_number();
						for(i = 1; i < 4; i++)
						{
							arg[i] = tok.next(cmd);
						}
						curve4(arg[0], arg[1], arg[2], arg[3], cmd == 's');
						break;

					case 'A': case 'a':
						throw exception("parse_path: Command A: NOT IMPLEMENTED YET");

					case 'Z': case 'z':
						close_subpath();
						break;

					default:
						{
							char buf[100];
							sprintf(buf, "parse_path: Invalid Command %c", cmd);
							throw exception(buf);
						}
					}
				}
			}

			void end_path()
			{
				if(m_attr_storage.size() == 0) 
				{
					throw exception("end_path : The path was not begun");
				}

				cur_path().attr = cur_attr();
			}



			// The following functions are essentially a "reflection" of
			// the respective SVG path commands.
			void move_to(Gdiplus::REAL x, Gdiplus::REAL y, bool rel=false)
			{
				cur_path().MoveTo(x, y, rel);
			}

			void line_to(Gdiplus::REAL x,  Gdiplus::REAL y, bool rel=false)
			{
				cur_path().LineTo(x, y, rel);
			}

			void hline_to(Gdiplus::REAL x, bool rel=false)
			{
				line_to(x, cur_path().point.Y, rel);
			}

			void vline_to(Gdiplus::REAL y, bool rel=false)
			{
				line_to(cur_path().point.X, y, rel);
			}

			void curve3(Gdiplus::REAL x1, Gdiplus::REAL y1, Gdiplus::REAL x,  Gdiplus::REAL y, bool rel=false)
			{
				cur_path().CurveTo3(x1, y1, x, y, rel);
			}

			void curve3(Gdiplus::REAL x, Gdiplus::REAL y, bool rel=false)
			{
				cur_path().CurveTo3(x, y, rel);
			}

			void curve4(Gdiplus::REAL x1, Gdiplus::REAL y1, Gdiplus::REAL x2, Gdiplus::REAL y2, Gdiplus::REAL x,  Gdiplus::REAL y, bool rel=false)
			{
				cur_path().CurveTo4(x1, y1, x2, y2, x, y, rel);
			}

			void curve4(Gdiplus::REAL x2, Gdiplus::REAL y2, Gdiplus::REAL x,  Gdiplus::REAL y, bool rel=false)
			{
				cur_path().CurveTo4(x2, y2, x, y, rel);
			}

			void close_subpath()
			{
				//cur_attr().pPath->CloseSubPath();
			}



			// Call these functions on <g> tag (start_element, end_element respectively)
			void push_attr()
			{
				m_attr_storage.push_back(path_attributes());
			}

			void pop_attr()
			{
				m_attr_storage.pop_back();
			}

			// Attribute setting functions.
			void fill(const Gdiplus::Color& f) { cur_attr().fill_color = f; cur_attr().fill_flag = true; };
			void stroke(const Gdiplus::Color& s) { cur_attr().stroke_color = s; cur_attr().stroke_flag = true;  };
			void even_odd(bool flag) { cur_attr().even_odd_flag = flag; };
			void stroke_width(Gdiplus::REAL w) { cur_attr().stroke_width = w; cur_attr().stroke_flag = true;  };
			void fill_none() { cur_attr().fill_flag = false; };
			void stroke_none() { cur_attr().stroke_flag = false; };
			void fill_opacity(Gdiplus::REAL op) {  cur_attr().fill_flag = true; };
			void stroke_opacity(Gdiplus::REAL op) {  cur_attr().stroke_flag = true;  };
			void line_join(Gdiplus::LineJoin join) { cur_attr().line_join = join; };
			void line_cap(Gdiplus::LineCap cap) { cur_attr().line_cap = cap; };
			void miter_limit(Gdiplus::REAL ml) { cur_attr().miter_limit = ml; };

			Gdiplus::Matrix& transform() { return *cur_attr().pTransform; };



		};



		class parser
		{


		public:

			//------------------------------------------------------------------------
			~parser()
			{
				delete [] m_attr_value;
				delete [] m_attr_name;
				delete [] m_buf;
				delete [] m_title;
			}

			//------------------------------------------------------------------------
			parser(path_renderer& path) :
			m_path(path),
				m_tokenizer(),
				m_buf(new char[buf_size]),
				m_title(new char[256]),
				m_title_len(0),
				m_title_flag(false),
				m_path_flag(false),
				m_attr_name(new char[128]),
				m_attr_value(new char[1024]),
				m_attr_name_len(127),
				m_attr_value_len(1023)
			{
				m_title[0] = 0;
			}

			//------------------------------------------------------------------------
			void parse(const char* fname)
			{
				char msg[1024];
				XML_Parser p = XML_ParserCreate(NULL);
				if(p == 0) 
				{
					throw exception("Couldn't allocate memory for parser");
				}

				XML_SetUserData(p, this);
				XML_SetElementHandler(p, start_element, end_element);
				XML_SetCharacterDataHandler(p, content);

				FILE* fd = fopen(fname, "rt");
				if(fd == 0)
				{
					sprintf(msg, "Couldn't open file %s", fname);
					throw exception(msg);
				}

				bool done = false;
				do
				{
					size_t len = fread(m_buf, 1, buf_size, fd);
					done = len < buf_size;
					if(!XML_Parse(p, m_buf, len, done))
					{
						sprintf(msg,
							"%s at line %d\n",
							XML_ErrorString(XML_GetErrorCode(p)),
							XML_GetCurrentLineNumber(p));
						throw exception(msg);
					}
				}
				while(!done);
				fclose(fd);
				XML_ParserFree(p);

				char* ts = m_title;
				while(*ts)
				{
					if(*ts < ' ') *ts = ' ';
					++ts;
				}
			}

		private:

			enum { buf_size = BUFSIZ };

			path_renderer& m_path;
			path_tokenizer m_tokenizer;
			char*          m_buf;
			char*          m_title;
			unsigned       m_title_len;
			bool           m_title_flag;
			bool           m_path_flag;
			char*          m_attr_name;
			char*          m_attr_value;
			unsigned       m_attr_name_len;
			unsigned       m_attr_value_len;





			//------------------------------------------------------------------------
			static void start_element(void* data, const char* el, const char** attr)
			{
				parser& self = *(parser*)data;

				if(stricmp(el, "title") == 0)
				{
					self.m_title_flag = true;
				}
				else if(stricmp(el, "g") == 0)
				{
					self.m_path.push_attr();
					self.parse_attr(attr);
				}
				else if(stricmp(el, "path") == 0)
				{
					if(self.m_path_flag)
					{
						throw exception("start_element: Nested path");
					}
					self.m_path.begin_path();
					self.parse_path(attr);
					self.m_path.end_path();
					self.m_path_flag = true;
				}
				else if(stricmp(el, "rect") == 0) 
				{
					self.parse_rect(attr);
				}
				else if(stricmp(el, "line") == 0) 
				{
					self.parse_line(attr);
				}
				else if(stricmp(el, "polyline") == 0) 
				{
					self.parse_poly(attr, false);
				}
				else if(stricmp(el, "polygon") == 0) 
				{
					self.parse_poly(attr, true);
				}
				//else
				//if(stricmp(el, "<OTHER_ELEMENTS>") == 0) 
				//{
				//}
				// . . .
			} 


			//------------------------------------------------------------------------
			static void end_element(void* data, const char* el)
			{
				parser& self = *(parser*)data;

				if(stricmp(el, "title") == 0)
				{
					self.m_title_flag = false;
				}
				else if(stricmp(el, "g") == 0)
				{
					self.m_path.pop_attr();
				}
				else if(stricmp(el, "path") == 0)
				{
					self.m_path_flag = false;
				}
				//else
				//if(stricmp(el, "<OTHER_ELEMENTS>") == 0) 
				//{
				//}
				// . . .
			}


			//------------------------------------------------------------------------
			static void content(void* data, const char* s, int len)
			{
				parser& self = *(parser*)data;

				// m_title_flag signals that the <title> tag is being parsed now.
				// The following code concatenates the pieces of content of the <title> tag.
				if(self.m_title_flag)
				{
					if(len + self.m_title_len > 255) len = 255 - self.m_title_len;
					if(len > 0) 
					{
						memcpy(self.m_title + self.m_title_len, s, len);
						self.m_title_len += len;
						self.m_title[self.m_title_len] = 0;
					}
				}
			}


			//------------------------------------------------------------------------
			void parse_attr(const char** attr)
			{
				int i;
				for(i = 0; attr[i]; i += 2)
				{
					if(stricmp(attr[i], "style") == 0)
					{
						parse_style(attr[i + 1]);
					}
					else
					{
						parse_attr(attr[i], attr[i + 1]);
					}
				}
			}

			//-------------------------------------------------------------
			void parse_path(const char** attr)
			{
				int i;

				for(i = 0; attr[i]; i += 2)
				{
					// The <path> tag can consist of the path itself ("d=") 
					// as well as of other parameters like "style=", "transform=", etc.
					// In the last case we simply rely on the function of parsing 
					// attributes (see 'else' branch).
					if(stricmp(attr[i], "d") == 0)
					{
						m_tokenizer.set_path_str(attr[i + 1]);
						m_path.parse_path(m_tokenizer);
					}
					else
					{
						// Create a temporary single pair "name-value" in order
						// to avoid multiple calls for the same attribute.
						const char* tmp[4];
						tmp[0] = attr[i];
						tmp[1] = attr[i + 1];
						tmp[2] = 0;
						tmp[3] = 0;
						parse_attr(tmp);
					}
				}
			}

			struct named_color
			{
				char  name[64];
				Gdiplus::ARGB clr;
			};


			//-------------------------------------------------------------
			static int cmp_color(const void* p1, const void* p2)
			{
				return stricmp(((named_color*)p1)->name, ((named_color*)p2)->name);
			}

			//-------------------------------------------------------------
			Gdiplus::Color parse_color(const char* str)
			{
				named_color colors[] = 
				{
					"AliceBlue", Gdiplus::Color::AliceBlue, 
						"AntiqueWhite", Gdiplus::Color::AntiqueWhite,
						"Aqua", Gdiplus::Color::Aqua,
						"Aquamarine", Gdiplus::Color::Aquamarine,
						"Azure", Gdiplus::Color::Azure,
						"Beige", Gdiplus::Color::Beige,
						"Bisque", Gdiplus::Color::Bisque,
						"Black", Gdiplus::Color::Black,
						"BlanchedAlmond", Gdiplus::Color::BlanchedAlmond,
						"Blue", Gdiplus::Color::Blue,
						"BlueViolet", Gdiplus::Color::BlueViolet,
						"Brown", Gdiplus::Color::Brown,
						"BurlyWood", Gdiplus::Color::BurlyWood,
						"CadetBlue", Gdiplus::Color::CadetBlue,
						"Chartreuse", Gdiplus::Color::Chartreuse,
						"Chocolate", Gdiplus::Color::Chocolate,
						"Coral", Gdiplus::Color::Coral,
						"CornflowerBlue", Gdiplus::Color::CornflowerBlue,
						"Cornsilk", Gdiplus::Color::Cornsilk,
						"Crimson", Gdiplus::Color::Crimson,
						"Cyan", Gdiplus::Color::Cyan,
						"DarkBlue", Gdiplus::Color::DarkBlue,
						"DarkCyan", Gdiplus::Color::DarkCyan,
						"DarkGoldenrod", Gdiplus::Color::DarkGoldenrod,
						"DarkGray", Gdiplus::Color::DarkGray,
						"DarkGreen", Gdiplus::Color::DarkGreen,
						"DarkKhaki", Gdiplus::Color::DarkKhaki,
						"DarkMagenta", Gdiplus::Color::DarkMagenta,
						"DarkOliveGreen", Gdiplus::Color::DarkOliveGreen,
						"DarkOrange", Gdiplus::Color::DarkOrange,
						"DarkOrchid", Gdiplus::Color::DarkOrchid,
						"DarkRed", Gdiplus::Color::DarkRed,
						"DarkSalmon", Gdiplus::Color::DarkSalmon,
						"DarkSeaGreen", Gdiplus::Color::DarkSeaGreen,
						"DarkSlateBlue", Gdiplus::Color::DarkSlateBlue,
						"DarkSlateGray", Gdiplus::Color::DarkSlateGray,
						"DarkTurquoise", Gdiplus::Color::DarkTurquoise,
						"DarkViolet", Gdiplus::Color::DarkViolet,
						"DeepPink", Gdiplus::Color::DeepPink,
						"DeepSkyBlue", Gdiplus::Color::DeepSkyBlue,
						"DimGray", Gdiplus::Color::DimGray,
						"DodgerBlue", Gdiplus::Color::DodgerBlue,
						"Firebrick", Gdiplus::Color::Firebrick,
						"FloralWhite", Gdiplus::Color::FloralWhite,
						"ForestGreen", Gdiplus::Color::ForestGreen,
						"Fuchsia", Gdiplus::Color::Fuchsia,
						"Gainsboro", Gdiplus::Color::Gainsboro,
						"GhostWhite", Gdiplus::Color::GhostWhite,
						"Gold", Gdiplus::Color::Gold,
						"Goldenrod", Gdiplus::Color::Goldenrod,
						"Gray", Gdiplus::Color::Gray,
						"Green", Gdiplus::Color::Green,
						"GreenYellow", Gdiplus::Color::GreenYellow,
						"Honeydew", Gdiplus::Color::Honeydew,
						"HotPink", Gdiplus::Color::HotPink,
						"IndianRed", Gdiplus::Color::IndianRed,
						"Indigo", Gdiplus::Color::Indigo,
						"Ivory", Gdiplus::Color::Ivory,
						"Khaki", Gdiplus::Color::Khaki,
						"Lavender", Gdiplus::Color::Lavender,
						"LavenderBlush", Gdiplus::Color::LavenderBlush,
						"LawnGreen", Gdiplus::Color::LawnGreen,
						"LemonChiffon", Gdiplus::Color::LemonChiffon,
						"LightBlue", Gdiplus::Color::LightBlue,
						"LightCoral", Gdiplus::Color::LightCoral,
						"LightCyan", Gdiplus::Color::LightCyan,
						"LightGoldenrodYellow", Gdiplus::Color::LightGoldenrodYellow,
						"LightGray", Gdiplus::Color::LightGray,
						"LightGreen", Gdiplus::Color::LightGreen,
						"LightPink", Gdiplus::Color::LightPink,
						"LightSalmon", Gdiplus::Color::LightSalmon,
						"LightSeaGreen", Gdiplus::Color::LightSeaGreen,
						"LightSkyBlue", Gdiplus::Color::LightSkyBlue,
						"LightSlateGray", Gdiplus::Color::LightSlateGray,
						"LightSteelBlue", Gdiplus::Color::LightSteelBlue,
						"LightYellow", Gdiplus::Color::LightYellow,
						"Lime", Gdiplus::Color::Lime,
						"LimeGreen", Gdiplus::Color::LimeGreen,
						"Linen", Gdiplus::Color::Linen,
						"Magenta", Gdiplus::Color::Magenta,
						"Maroon", Gdiplus::Color::Maroon,
						"MediumAquamarine", Gdiplus::Color::MediumAquamarine,
						"MediumBlue", Gdiplus::Color::MediumBlue,
						"MediumOrchid", Gdiplus::Color::MediumOrchid,
						"MediumPurple", Gdiplus::Color::MediumPurple,
						"MediumSeaGreen", Gdiplus::Color::MediumSeaGreen,
						"MediumSlateBlue", Gdiplus::Color::MediumSlateBlue,
						"MediumSpringGreen", Gdiplus::Color::MediumSpringGreen,
						"MediumTurquoise", Gdiplus::Color::MediumTurquoise,
						"MediumVioletRed", Gdiplus::Color::MediumVioletRed,
						"MidnightBlue", Gdiplus::Color::MidnightBlue,
						"MintCream", Gdiplus::Color::MintCream,
						"MistyRose", Gdiplus::Color::MistyRose,
						"Moccasin", Gdiplus::Color::Moccasin,
						"NavajoWhite", Gdiplus::Color::NavajoWhite,
						"Navy", Gdiplus::Color::Navy,
						"OldLace", Gdiplus::Color::OldLace,
						"Olive", Gdiplus::Color::Olive,
						"OliveDrab", Gdiplus::Color::OliveDrab,
						"Orange", Gdiplus::Color::Orange,
						"OrangeRed", Gdiplus::Color::OrangeRed,
						"Orchid", Gdiplus::Color::Orchid,
						"PaleGoldenrod", Gdiplus::Color::PaleGoldenrod,
						"PaleGreen", Gdiplus::Color::PaleGreen,
						"PaleTurquoise", Gdiplus::Color::PaleTurquoise,
						"PaleVioletRed", Gdiplus::Color::PaleVioletRed,
						"PapayaWhip", Gdiplus::Color::PapayaWhip,
						"PeachPuff", Gdiplus::Color::PeachPuff,
						"Peru", Gdiplus::Color::Peru,
						"Pink", Gdiplus::Color::Pink,
						"Plum", Gdiplus::Color::Plum,
						"PowderBlue", Gdiplus::Color::PowderBlue,
						"Purple", Gdiplus::Color::Purple,
						"Red", Gdiplus::Color::Red,
						"RosyBrown", Gdiplus::Color::RosyBrown,
						"RoyalBlue", Gdiplus::Color::RoyalBlue,
						"SaddleBrown", Gdiplus::Color::SaddleBrown,
						"Salmon", Gdiplus::Color::Salmon,
						"SandyBrown", Gdiplus::Color::SandyBrown,
						"SeaGreen", Gdiplus::Color::SeaGreen,
						"SeaShell", Gdiplus::Color::SeaShell,
						"Sienna", Gdiplus::Color::Sienna,
						"Silver", Gdiplus::Color::Silver,
						"SkyBlue", Gdiplus::Color::SkyBlue,
						"SlateBlue", Gdiplus::Color::SlateBlue,
						"SlateGray", Gdiplus::Color::SlateGray,
						"Snow", Gdiplus::Color::Snow,
						"SpringGreen", Gdiplus::Color::SpringGreen,
						"SteelBlue", Gdiplus::Color::SteelBlue,
						"Tan", Gdiplus::Color::Tan,
						"Teal", Gdiplus::Color::Teal,
						"Thistle", Gdiplus::Color::Thistle,
						"Tomato", Gdiplus::Color::Tomato,
						"Transparent", Gdiplus::Color::Transparent,
						"Turquoise", Gdiplus::Color::Turquoise,
						"Violet", Gdiplus::Color::Violet,
						"Wheat", Gdiplus::Color::Wheat,
						"White", Gdiplus::Color::White,
						"WhiteSmoke", Gdiplus::Color::WhiteSmoke,
						"Yellow", Gdiplus::Color::Yellow,
						"YellowGreen", Gdiplus::Color::YellowGreen,
						"AAAA", 0
				}; 

				while(*str == ' ') ++str;
				unsigned c = 0;
				if(*str == '#')
				{
					sscanf(str + 1, "%X", &c);
					return Gdiplus::Color(c | 0xff000000);
				}
				else
				{
					named_color c;
					unsigned len = strlen(str);
					if(len > sizeof(c.name) - 1)
					{
						CString str;
						str.Format("parse_color: Invalid color name '%s'", str);
						throw exception(str);
					}
					strcpy(c.name, str);
					const void* p = bsearch(&c, colors, sizeof(colors) / sizeof(colors[0]), sizeof(colors[0]), cmp_color);

					if(p == 0)
					{
						//throw exception("parse_color: Invalid color name '%s'", str);
						return Gdiplus::Color(255, 255, 255, 255); // Return default color
					}
					const named_color* pc = (const named_color*)p;
					return Gdiplus::Color(pc->clr);
				}
			}

			Gdiplus::REAL parse_real(const char* str)
			{
				while(*str == ' ') ++str;
				return static_cast<float>(atof(str));
			}



			//-------------------------------------------------------------
			bool parse_attr(const char* name, const char* value)
			{
				if(stricmp(name, "style") == 0)
				{
					parse_style(value);
				}
				else if(stricmp(name, "fill") == 0)
				{
					if(stricmp(value, "none") == 0)
					{
						m_path.fill_none();
					}
					else
					{
						m_path.fill(parse_color(value));
					}
				}
				else if(stricmp(name, "fill-opacity") == 0)
				{
					m_path.fill_opacity(parse_real(value));
				}
				else if(stricmp(name, "stroke") == 0)
				{
					if(stricmp(value, "none") == 0)
					{
						m_path.stroke_none();
					}
					else
					{
						m_path.stroke(parse_color(value));
					}
				}
				else if(stricmp(name, "stroke-width") == 0)
				{
					m_path.stroke_width(parse_real(value));
				}
				else if(stricmp(name, "stroke-linecap") == 0)
				{
					if(stricmp(value, "butt") == 0)        m_path.line_cap(Gdiplus::LineCapFlat);
					else if(stricmp(value, "round") == 0)  m_path.line_cap(Gdiplus::LineCapRound);
					else if(stricmp(value, "square") == 0) m_path.line_cap(Gdiplus::LineCapSquare);
				}
				else if(stricmp(name, "stroke-linejoin") == 0)
				{
					if(stricmp(value, "miter") == 0)      m_path.line_join(Gdiplus::LineJoinMiter);
					else if(stricmp(value, "round") == 0) m_path.line_join(Gdiplus::LineJoinRound);
					else if(stricmp(value, "bevel") == 0) m_path.line_join(Gdiplus::LineJoinBevel);
				}
				else if(stricmp(name, "stroke-miterlimit") == 0)
				{
					m_path.miter_limit(parse_real(value));
				}
				else if(stricmp(name, "stroke-opacity") == 0)
				{
					m_path.stroke_opacity(parse_real(value));
				}
				else if(stricmp(name, "transform") == 0)
				{
					parse_transform(value);
				}
				//else
				//if(stricmp(el, "<OTHER_ATTRIBUTES>") == 0) 
				//{
				//}
				// . . .
				else
				{
					return false;
				}
				return true;
			}



			//-------------------------------------------------------------
			void copy_name(const char* start, const char* end)
			{
				unsigned len = unsigned(end - start);
				if(m_attr_name_len == 0 || len > m_attr_name_len)
				{
					delete [] m_attr_name;
					m_attr_name = new char[len + 1];
					m_attr_name_len = len;
				}
				if(len) memcpy(m_attr_name, start, len);
				m_attr_name[len] = 0;
			}



			//-------------------------------------------------------------
			void copy_value(const char* start, const char* end)
			{
				unsigned len = unsigned(end - start);
				if(m_attr_value_len == 0 || len > m_attr_value_len)
				{
					delete [] m_attr_value;
					m_attr_value = new char[len + 1];
					m_attr_value_len = len;
				}
				if(len) memcpy(m_attr_value, start, len);
				m_attr_value[len] = 0;
			}


			//-------------------------------------------------------------
			bool parse_name_value(const char* nv_start, const char* nv_end)
			{
				const char* str = nv_start;
				while(str < nv_end && *str != ':') ++str;

				const char* val = str;

				// Right Trim
				while(str > nv_start && 
					(*str == ':' || isspace(*str))) --str;
				++str;

				copy_name(nv_start, str);

				while(val < nv_end && (*val == ':' || isspace(*val))) ++val;

				copy_value(val, nv_end);
				return parse_attr(m_attr_name, m_attr_value);
			}



			//-------------------------------------------------------------
			void parse_style(const char* str)
			{
				while(*str)
				{
					// Left Trim
					while(*str && isspace(*str)) ++str;
					const char* nv_start = str;
					while(*str && *str != ';') ++str;
					const char* nv_end = str;

					// Right Trim
					while(nv_end > nv_start && 
						(*nv_end == ';' || isspace(*nv_end))) --nv_end;
					++nv_end;

					parse_name_value(nv_start, nv_end);
					if(*str) ++str;
				}

			}


			//-------------------------------------------------------------
			void parse_rect(const char** attr)
			{
				int i;
				Gdiplus::REAL x = 0.0;
				Gdiplus::REAL y = 0.0;
				Gdiplus::REAL w = 0.0;
				Gdiplus::REAL h = 0.0;
				Gdiplus::REAL rx = 0.0;
				Gdiplus::REAL ry = 0.0;

				m_path.begin_path();
				for(i = 0; attr[i]; i += 2)
				{
					if(!parse_attr(attr[i], attr[i + 1]))
					{
						if(stricmp(attr[i], "x") == 0)      x = parse_real(attr[i + 1]);
						if(stricmp(attr[i], "y") == 0)      y = parse_real(attr[i + 1]);
						if(stricmp(attr[i], "width") == 0)  w = parse_real(attr[i + 1]);
						if(stricmp(attr[i], "height") == 0) h = parse_real(attr[i + 1]);
						//ZW: rx, ry for rounded rects
						if(stricmp(attr[i], "rx") == 0)      rx = parse_real(attr[i + 1]);
						if(stricmp(attr[i], "ry") == 0)      ry = parse_real(attr[i + 1]);
					}
				}


				if(w != 0.0 && h != 0.0)
				{
					if(w < 0.0) throw exception("parse_rect: Invalid width");
					if(h < 0.0) throw exception("parse_rect: Invalid height");

					//ZW: rx, ry for rounded rects
					if (rx != 0.0 || ry != 0.0)
					{
						// Creating a rounded rectangle
						//TODO agg::rounded_rect r(x, y, x + w, y + h, (rx != 0.0) ? rx : ry);
						//if (rx != 0.0 && ry != 0.0) r.radius(rx, ry);
						//r.normalize_radius();
						//m_path.add_path(r);
					}
					else
					{
						m_path.move_to(x,     y);
						m_path.line_to(x + w, y);
						m_path.line_to(x + w, y + h);
						m_path.line_to(x,     y + h);
						m_path.close_subpath();
					}
				}
				m_path.end_path();
			}


			//-------------------------------------------------------------
			void parse_line(const char** attr)
			{
				int i;
				Gdiplus::REAL x1 = 0.0;
				Gdiplus::REAL y1 = 0.0;
				Gdiplus::REAL x2 = 0.0;
				Gdiplus::REAL y2 = 0.0;

				m_path.begin_path();
				for(i = 0; attr[i]; i += 2)
				{
					if(!parse_attr(attr[i], attr[i + 1]))
					{
						if(stricmp(attr[i], "x1") == 0) x1 = parse_real(attr[i + 1]);
						if(stricmp(attr[i], "y1") == 0) y1 = parse_real(attr[i + 1]);
						if(stricmp(attr[i], "x2") == 0) x2 = parse_real(attr[i + 1]);
						if(stricmp(attr[i], "y2") == 0) y2 = parse_real(attr[i + 1]);
					}
				}

				m_path.move_to(x1, y1);
				m_path.line_to(x2, y2);
				m_path.end_path();
			}


			//-------------------------------------------------------------
			void parse_poly(const char** attr, bool close_flag)
			{
				int i;
				Gdiplus::REAL x = 0.0;
				Gdiplus::REAL y = 0.0;

				m_path.begin_path();
				for(i = 0; attr[i]; i += 2)
				{
					if(!parse_attr(attr[i], attr[i + 1]))
					{
						if(stricmp(attr[i], "points") == 0) 
						{
							m_tokenizer.set_path_str(attr[i + 1]);
							if(!m_tokenizer.next())
							{
								throw exception("parse_poly: Too few coordinates");
							}
							x = m_tokenizer.last_number();
							if(!m_tokenizer.next())
							{
								throw exception("parse_poly: Too few coordinates");
							}
							y = m_tokenizer.last_number();
							m_path.move_to(x, y);
							while(m_tokenizer.next())
							{
								x = m_tokenizer.last_number();
								if(!m_tokenizer.next())
								{
									throw exception("parse_poly: Odd number of coordinates");
								}
								y = m_tokenizer.last_number();
								m_path.line_to(x, y);
							}
						}
					}
				}
				m_path.end_path();
			}

			//-------------------------------------------------------------
			void parse_transform(const char* str)
			{
				while(*str)
				{
					if(islower(*str))
					{
						if(strncmp(str, "matrix", 6) == 0)    str += parse_matrix(str);    
						else if(strncmp(str, "translate", 9) == 0) str += parse_translate(str);
						else if(strncmp(str, "rotate", 6) == 0)    str += parse_rotate(str);    
						else if(strncmp(str, "scale", 5) == 0)     str += parse_scale(str);     
						else  if(strncmp(str, "skewX", 5) == 0)     str += parse_skew_x(str);    
						else if(strncmp(str, "skewY", 5) == 0)     str += parse_skew_y(str);    
						else
						{
							++str;
						}
					}
					else
					{
						++str;
					}
				}
			}


			//-------------------------------------------------------------
			static bool is_numeric(char c)
			{
				return strchr("0123456789+-.eE", c) != 0;
			}

			//-------------------------------------------------------------
			static unsigned parse_transform_args(const char* str, 
				Gdiplus::REAL* args, 
				unsigned max_na, 
				unsigned* na)
			{
				*na = 0;
				const char* ptr = str;
				while(*ptr && *ptr != '(') ++ptr;
				if(*ptr == 0)
				{
					throw exception("parse_transform_args: Invalid syntax");
				}
				const char* end = ptr;
				while(*end && *end != ')') ++end;
				if(*end == 0)
				{
					throw exception("parse_transform_args: Invalid syntax");
				}

				while(ptr < end)
				{
					if(is_numeric(*ptr))
					{
						if(*na >= max_na)
						{
							throw exception("parse_transform_args: Too many arguments");
						}
						args[(*na)++] = static_cast<float>(atof(ptr));
						while(ptr < end && is_numeric(*ptr)) ++ptr;
					}
					else
					{
						++ptr;
					}
				}
				return unsigned(end - str);
			}

			//-------------------------------------------------------------
			unsigned parse_matrix(const char* str)
			{
				Gdiplus::REAL args[6];
				unsigned na = 0;
				unsigned len = parse_transform_args(str, args, 6, &na);
				if(na != 6)
				{
					throw exception("parse_matrix: Invalid number of arguments");
				}
				m_path.transform().SetElements(args[0], args[1], args[2], args[3], args[4], args[5]);
				return len;
			}

			//-------------------------------------------------------------
			unsigned parse_translate(const char* str)
			{
				Gdiplus::REAL args[2];
				unsigned na = 0;
				unsigned len = parse_transform_args(str, args, 2, &na);
				if(na == 1) args[1] = 0.0;
				m_path.transform().Translate(args[0], args[1]);
				return len;
			}

			//-------------------------------------------------------------
			unsigned parse_rotate(const char* str)
			{
				Gdiplus::REAL args[3];
				unsigned na = 0;
				unsigned len = parse_transform_args(str, args, 3, &na);
				if(na == 1) 
				{
					m_path.transform().Rotate(args[0]);
				}
				else if(na == 3)
				{
					m_path.transform().Translate(-args[1], -args[2]);
					m_path.transform().Rotate(args[0]);
					m_path.transform().Translate(args[1], args[2]);
				}
				else
				{
					throw exception("parse_rotate: Invalid number of arguments");
				}
				return len;
			}

			//-------------------------------------------------------------
			unsigned parse_scale(const char* str)
			{
				Gdiplus::REAL args[2];
				unsigned na = 0;
				unsigned len = parse_transform_args(str, args, 2, &na);
				if(na == 1) args[1] = args[0];
				m_path.transform().Scale(args[0], args[1]);
				return len;
			}

			//-------------------------------------------------------------
			unsigned parse_skew_x(const char* str)
			{
				Gdiplus::REAL arg;
				unsigned na = 0;
				unsigned len = parse_transform_args(str, &arg, 1, &na);
				m_path.transform().Shear(arg, 0.0);
				return len;
			}

			//-------------------------------------------------------------
			unsigned parse_skew_y(const char* str)
			{
				Gdiplus::REAL arg;
				unsigned na = 0;
				unsigned len = parse_transform_args(str, &arg, 1, &na);
				m_path.transform().Shear(0.0, arg);
				return len;
			}

		};
	}
}