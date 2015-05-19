#ifndef ViewImage_h
#define ViewImage_h

#include <stdlib.h>
#include <assert.h>
#include <math.h>

namespace sigverse
{

	//typedef unsigned short ImageDataType;

	enum ImageDataType{
		IMAGE_DATA_TYPE_ANY = 0,
		IMAGE_DATA_WINDOWS_BMP,
	};


	//typedef unsigned short ColorBitType;

	enum ColorBitType{
		COLORBIT_ANY = 0,
		COLORBIT_24,
		DEPTHBIT_8,
	};

	enum ImageDataSize {
		IMAGE_320X240 = 0,
		IMAGE_320X1,
	};

	/**
	* @brief �摜�f�[�^���N���X
	*/
	class ViewImageInfo
	{
	private:
		ImageDataType	m_dataType;
		ColorBitType	m_cbType;
		int		m_width;
		int		m_height;

	public:
		/**
		* @brief �R���X�g���N�^
		*
		* @param dataType �摜�f�[�^�^�C�v
		* @param cbType   �J���[�r�b�g�^�C�v
		* @param sz       �摜�T�C�Y
		*/
		ViewImageInfo(ImageDataType dataType, ColorBitType cbType, ImageDataSize sz) : m_dataType(dataType), m_cbType(cbType)
		{
		
			switch(sz) {
	case IMAGE_320X240:
		m_width = 320; m_height = 240;
		break;
	case IMAGE_320X1:
		m_width = 320; m_height = 1;
		break;
	default:
		assert(0);
		break;
			}
		}
		/**
		* @brief �R���X�g���N�^
		*
		* @param dataType �摜�f�[�^�^�C�v
		* @param cbType   �J���[�r�b�g�^�C�v
		* @param w        ��(pixel)
		* @param h        ����(pixel)
		*/
		ViewImageInfo(ImageDataType dataType, ColorBitType cbType, int w, int h)
			: m_dataType(dataType), m_cbType(cbType), m_width(w), m_height(h)
		{
		}
		//! �R�s�[�R���X�g���N�^
		ViewImageInfo(const ViewImageInfo &o)
			: m_dataType(o.m_dataType),
			m_cbType(o.m_cbType),
			m_width(o.m_width), m_height(o.m_height)
		{
		}
		//! �摜�f�[�^�`��(BMP etc...)�𓾂�
		ImageDataType getDataType() const { return m_dataType; }
		//! �J���[�^�C�v�𓾂�
		ColorBitType  getColorBitType() const { return m_cbType; }
		//! �摜�̕�(pixel)�𓾂�
		int	      getWidth() const { return m_width; }
		//! �摜�̍���(pixel)�𓾂�
		int	      getHeight() const { return m_height; }

		//! 1�s�N�Z��������̃f�[�^�T�C�Y�𓾂�
		int	getBytesPerOnePixel() const {
			int b;
			switch(m_cbType) {
	case COLORBIT_24:
		b = 3; break;
	case DEPTHBIT_8:
		b = 1; break;
	default:
		assert(0);	// error
		b = 0; break;
			}

			return b;
		}
	};


	/**
	* @brief ���o�f�[�^�N���X
	*/
	class ViewImage
	{
	private:
		ViewImageInfo	m_info;
		char *		m_buf;
		int		m_buflen;
	
		double m_fov;
		double m_ar;   //aspect ratio
	public:
		/**
		*  @brief �R���X�g���N�^
		*
		*  @param info ���o�f�[�^���
		*/
		ViewImage(const ViewImageInfo &info)
			: m_info(info), m_buf(0), m_buflen(0)
		{
			m_buflen = calcBufferSize(info);
			m_buf = new char [m_buflen];
		}

		//! �f�X�g���N�^
		~ViewImage() {
			if (m_buf) {
				delete [] m_buf; m_buf = 0;
			}
		}

	private:
		// sekikawa(2007/10/12)
		int getWidthBytes(int width, int bytesPerOnePixel);

		int	calcBufferSize(const ViewImageInfo &info);

	public:
		//! ���o�f�[�^���̎擾
		const ViewImageInfo & getInfo() { return m_info; }

		//! �摜�̕�(pixel)�̎擾
		int  getWidth() const { return m_info.getWidth(); }
		//! �摜�̍���(pixel)�̎擾
		int  getHeight() const { return m_info.getHeight(); }
		//! �摜�f�[�^�̎擾
		char * getBuffer() const { return m_buf; }
		//! �摜�f�[�^�ݒ�
		void setBuffer(char *buf) { m_buf = buf; }
		//! �摜�f�[�^���̎擾
		int   getBufferLength() const { return m_buflen; }
		//! ����p�ݒ�
		void setFOVy(double fov){ m_fov = fov;} 
		//! �A�X�y�N�g��ݒ�
		void setAspectRatio(double ar){ m_ar = ar;}
		//! ����p�擾(y����)
		double getFOVy(){ return m_fov;} 
		//! ����p�擾(x����)
		double getFOVx(){ return 2*atan(tan(m_fov*0.5)*m_ar);} 
		//! �A�X�y�N�g��擾
		double getAspectRatio(){ return m_ar;}

		//virtual void setDimension(int n){} 

        // sekikawa(2007/10/12)
        // convert RGBA format to BGR and turn y-axis upside down
        void setBitImageAsWindowsBMP(unsigned char *bitImage);

        /**                                                                                                                                                                                                  
         * @brief �摜�f�[�^�� Windows �r�b�g�}�b�v�`���ŕۑ�����                                                                                                                                            
         *                                                                                                                                                                                                   
         * @param fname �ۑ���t�@�C����                                                                                                                                                                     
         */
        bool    saveAsWindowsBMP(const char *fname);

	};
} // namespace sigverse
#endif // ViewImage_h
 
