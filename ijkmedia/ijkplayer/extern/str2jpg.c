/**
 * \Copyright (C)
 * \file    config.h
 * \author
 * \date    2018
 * \version 1.0
 * \brief   idr create.\n
 *
 *                              
 * \n
 */

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include <stdio.h>
#include "../config.h"


#define FM_JPG_WIDTH     160
#define FM_JPG_HIGH      120
//strIdr2Jpg g_idr2jpg;



/**
 * \fn      scale_img
 * \brief   scale the image
 * \param   
 * \return  
 */
int scale_img(AVCodecContext *pCodecCtx,AVFrame *src_picture,AVFrame *dst_picture,int nDstW,int nDstH)
{
	int i ;
	int nSrcStride[3];
	int nDstStride[3];
	int nSrcH = pCodecCtx->height;
	int nSrcW = pCodecCtx->width;
	uint8_t *pSrcBuff[3] = {src_picture->data[0],src_picture->data[1], src_picture->data[2]};
	struct SwsContext* m_pSwsContext;

	nSrcStride[0] = nSrcW ;
	nSrcStride[1] = nSrcW/2 ;
	nSrcStride[2] = nSrcW/2 ;
	dst_picture->linesize[0] = nDstW;
	dst_picture->linesize[1] = nDstW / 2;
	dst_picture->linesize[2] = nDstW / 2;
	
    LOGD("aizj enter %s,line=%d",__FUNCTION__,__LINE__);

	m_pSwsContext = sws_getContext(nSrcW, nSrcH,  AV_PIX_FMT_YUV420P,
									nDstW, nDstH, AV_PIX_FMT_YUV420P,
									SWS_BICUBIC,
									NULL, NULL, NULL);
	if (NULL == m_pSwsContext)
	{
		av_log(NULL, AV_LOG_ERROR,"aizj enter  m_pSwsContext is NULL %s,line=%d",__FUNCTION__,__LINE__);
		exit (-1);
	}
	sws_scale(m_pSwsContext, src_picture->data,src_picture->linesize, 0, pCodecCtx->height,dst_picture->data,dst_picture->linesize);
	LOGD("line0:%d line1:%d line2:%d\n",dst_picture->linesize[0] ,dst_picture->linesize[1] ,dst_picture->linesize[2]);
	sws_freeContext(m_pSwsContext);
	return 1 ;
}

/**
 * \fn      yuv2jpg
 * \brief   save yuvimg to jpg
 * \param   
 * \return  0 ok; else faliled
 */
int  yuv2jpg(AVFrame *in_pFrame,int out_width,int out_high,const char* out_pFile) 
{
	AVFormatContext* pFormatCtx = NULL;
	AVOutputFormat* fmt = NULL;
	AVStream* video_st = NULL;
	AVCodecContext* pCodecCtx = NULL;
	AVCodec* pCodec = NULL;
	uint8_t* picture_buf = NULL;
	AVFrame* picture = NULL;
	AVPacket pkt;
	int got_picture = 0;
	int size;
	int iRet = 0;

	LOGD("aizj enter %s,line=%d,%s",__FUNCTION__,__LINE__,out_pFile);
	
	pFormatCtx = avformat_alloc_context();    
    if( !pFormatCtx ){
          av_log(NULL, AV_LOG_ERROR,"aizj alloc NULL,exit %s,",__FUNCTION__);
          return -1;
    }

	//Guess format
	fmt = av_guess_format("mjpeg", NULL, NULL);         
	pFormatCtx->oformat = fmt;                    


	//Output URL
	LOGD("aizj Line=%d", __LINE__);
	if (avio_open(&pFormatCtx->pb,out_pFile, AVIO_FLAG_READ_WRITE) < 0){
		av_log(NULL, AV_LOG_ERROR,"Couldn't open output file:%s",out_pFile);
        iRet = -1;
		goto fail;
	}                                               

    LOGD("aizj Line=%d", __LINE__);
	video_st = avformat_new_stream(pFormatCtx, 0); 
	if (video_st==NULL){
		av_log(NULL, AV_LOG_ERROR,"aizj enter %s,line=%d",__FUNCTION__,__LINE__);
		iRet = -1;
		goto fail;
	}                                       
    LOGD("aizj enter %s,line=%d,",__FUNCTION__,__LINE__);

	pCodecCtx = video_st->codec; 
	pCodecCtx->codec_id = fmt->video_codec;   
	pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO; 
	pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;

	pCodecCtx->width = out_width;   
	pCodecCtx->height = out_high; 

	pCodecCtx->time_base.num = 1;  
	pCodecCtx->time_base.den = 30;   
    
	//Output some information
	av_dump_format(pFormatCtx, 0, out_pFile, 1);
	pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
	if (!pCodec){
		av_log(NULL, AV_LOG_ERROR,"codec not found,%s,line=%d",__FUNCTION__,__LINE__);
		iRet = -1;
		goto fail;
	}
	if (avcodec_open2(pCodecCtx, pCodec,NULL) < 0){
		av_log(NULL, AV_LOG_ERROR,"codec not open,%s,line=%d",__FUNCTION__,__LINE__);
		iRet = -1;
		goto fail;
	}
	picture = av_frame_alloc(); 
	size = avpicture_get_size(pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
	picture_buf = (uint8_t *)av_malloc(size);
	if (!picture_buf)
	{
		av_log(NULL, AV_LOG_ERROR,"%s,line=%d",__FUNCTION__,__LINE__);
		iRet = -1;
		goto fail;
	}
	avpicture_fill((AVPicture *)picture, picture_buf, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);

	//Write Header
	avformat_write_header(pFormatCtx,NULL);
	LOGD("%s,line=%d",__FUNCTION__,__LINE__);
	av_new_packet(&pkt,pCodecCtx->width * pCodecCtx->height*3);
	picture->data[0] = in_pFrame->data[0];  
	picture->data[1] = in_pFrame->data[1];
	picture->data[2] = in_pFrame->data[2];

	//Encode
	iRet = avcodec_encode_video2(pCodecCtx, &pkt,picture, &got_picture);
	if(iRet < 0){
		av_log(NULL, AV_LOG_ERROR,"can't open encode");
		iRet = -1;
		goto fail;
	}
	if ( got_picture == 1){
		pkt.stream_index = video_st->index;
		iRet = av_write_frame(pFormatCtx, &pkt);
	}
    av_packet_unref(&pkt);//av_free_packet(&pkt);

	//Write Trailer
	av_write_trailer(pFormatCtx);
	LOGD("%s,line=%d",__FUNCTION__,__LINE__);
	if (video_st){
		avcodec_close(video_st->codec);
		av_free(picture);
		av_free(picture_buf);
	}
    return 0; 
    
fail:
	avio_close(pFormatCtx->pb);
	avformat_free_context(pFormatCtx);
	return iRet;
}

/**
 * \fn      fm_stream2jpg
 * \brief   get jpg from stream
 * \param   
 * \return   0,  succeed;
            <0,  failed;
 */
int stream2jpg(const char *in_streamName,const char *out_jpgName) 
{
  AVFormatContext *pFormatCtx = NULL;
  int             i, videoStream;
  AVCodecContext  *pCodecCtx = NULL;
  AVCodec         *pCodec = NULL;
  AVFrame         *pFrame = NULL; 
  AVFrame         *pFrameRGB = NULL;
  AVFrame         *dst_picture = NULL;
  AVPacket        packet;
  int             frameFinished;
  int             numBytes;
  uint8_t         *buffer = NULL;
  int         	  iRet = 0;

  AVDictionary    *optionsDict = NULL;
  struct SwsContext      *sws_ctx = NULL;

  if( in_streamName == NULL || out_jpgName  == NULL){
      av_log(NULL, AV_LOG_INFO,"%s,dir is NULL,exit",__FUNCTION__);
      return -EINVAL;    
  }

  av_log(NULL, AV_LOG_INFO,"line=%d,%s,%s",__LINE__,in_streamName,out_jpgName);
  iRet = avformat_open_input(&pFormatCtx, in_streamName, NULL, NULL);
  if(iRet != 0)
  {
	  av_log(NULL, AV_LOG_ERROR,"open input exit,i=%d\n",iRet);
      return -EIO;
  }
  
  LOGD("%s,line=%d",__FUNCTION__,__LINE__);
  if(avformat_find_stream_info(pFormatCtx, NULL)<0) 
  {
	  av_log(NULL, AV_LOG_ERROR,"can't find stream info,exit%s,line=%d",__FUNCTION__,__LINE__);
	   return -EINVAL;
  }
  
  // Dump information about file onto standard error
  av_dump_format(pFormatCtx, 0, in_streamName, 0);
  
  // Find the first video stream
  videoStream = -1;
  for(i=0; i<pFormatCtx->nb_streams; i++)
     if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
      	videoStream = i;
      	break;
    }
  if(videoStream==-1){
	  av_log(NULL, AV_LOG_ERROR,"Didn't find a video stream,exit%s",__FUNCTION__);
	  return -ENOEXEC; 
  }  	 
  pCodecCtx=pFormatCtx->streams[videoStream]->codec;
  
  // Find the decoder for the video stream
  pCodec=avcodec_find_decoder(pCodecCtx->codec_id);  
  if(pCodec==NULL) {
	av_log(NULL, AV_LOG_ERROR,"Unsupported codec,exit%s,line=%d",__FUNCTION__,__LINE__);	
    return -ENODEV; 
  }
  
  LOGD("%s,line=%d",__FUNCTION__,__LINE__);
  if(avcodec_open2(pCodecCtx, pCodec, &optionsDict)<0){
	    av_log(NULL, AV_LOG_ERROR,"open codec failed. %s,line=%d",__FUNCTION__,__LINE__);
		return -ENODEV; 
  } 		
  pFrame = av_frame_alloc();
  if(pFrame == NULL){
	  av_log(NULL, AV_LOG_ERROR,"Allocate an AVFrame faile,exit%s,line=%d",__FUNCTION__,__LINE__);   
      return -ENOMEM;
  }
  dst_picture = av_frame_alloc();
  if ( !dst_picture ) {
	  av_log(NULL, AV_LOG_ERROR,"Allocate dst_picture faile,exit%s,line=%d",__FUNCTION__,__LINE__);   
      iRet = -ENOMEM;
      goto fail;
  }
  if(avpicture_alloc((AVPicture *)dst_picture, pCodecCtx->pix_fmt,pCodecCtx->width, pCodecCtx->height)<0){
		av_log(NULL, AV_LOG_ERROR,"Allocate dst_picture faile,exit%s,line=%d",__FUNCTION__,__LINE__);   
        iRet = -ENOMEM;
        goto fail;
  }
  
	 //Allocate an AVFrame structure
    LOGD("%s,line=%d",__FUNCTION__,__LINE__);
	pFrameRGB=av_frame_alloc();  
	if(pFrameRGB == NULL){
  		av_log(NULL, AV_LOG_ERROR,"Allocate an AVFrame faile,exit%s,line=%d",__FUNCTION__,__LINE__);   
        iRet = -ENOMEM;
        goto fail;
    }

  // Determine required buffer size and allocate buffer
  numBytes=avpicture_get_size(AV_PIX_FMT_YUV420P, pCodecCtx->width,pCodecCtx->height);
  buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));
  LOGD("Cw=%d,Ch=%d,%s,line=%d",pCodecCtx->width,pCodecCtx->height,__FUNCTION__,__LINE__);
  sws_ctx =sws_getContext(
        pCodecCtx->width,
        pCodecCtx->height,
        pCodecCtx->pix_fmt,
        FM_JPG_WIDTH, 
        FM_JPG_HIGH, 
        AV_PIX_FMT_YUV420P,
        SWS_BICUBIC,         
        NULL,
        NULL,
        NULL
    );
  
  // Assign appropriate parts of buffer to image planes in pFrameRGB
  // Note that pFrameRGB is an AVFrame, but AVFrame is a superset of AVPicture
  avpicture_fill((AVPicture *)pFrameRGB, buffer,AV_PIX_FMT_YUV420P,FM_JPG_WIDTH,FM_JPG_HIGH);
  
  // Read frames and save first five frames to disk
  while(av_read_frame(pFormatCtx, &packet) >=0) {
  	
	  // Is this a packet from the video stream?
	  LOGD("si=%d,vs=%d,%s,line=%d",packet.stream_index,videoStream,__FUNCTION__,__LINE__);
	  if(packet.stream_index == videoStream) 
	  	{

		  // Decode video frame
	      avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished,&packet);
		  LOGD("pFrame=%d,%s,line=%d",pFrame,__FUNCTION__,__LINE__);

		  // Did we get a video frame?
	      if(frameFinished) {	  	
		      LOGD("%s,line=%d",__FUNCTION__,__LINE__);
		      sws_scale
                (
		            sws_ctx,
		            (uint8_t const * const *)pFrame->data,
		            pFrame->linesize,
		            0,
		            pCodecCtx->height,
		            pFrameRGB->data,
		            pFrameRGB->linesize
		        );
				//fm_dbg_log(NULL, AV_LOG_INFO,"iRet=%d,%s,line=%d",iRet,__FUNCTION__,__LINE__);
			  	iRet = yuv2jpg(pFrameRGB,FM_JPG_WIDTH,FM_JPG_HIGH,out_jpgName);
			    //fm_dbg_log(NULL, AV_LOG_INFO,"%s,line=%d",__FUNCTION__,__LINE__);
			    break;
	      }
	    }
    
    	// Free the packet that was allocated by av_read_frame
    	av_free_packet(&packet);
  } 
  
fail:
  av_free(buffer);
  av_free(pFrameRGB); 
  av_free(pFrame);
  sws_freeContext(sws_ctx);
  avcodec_close(pCodecCtx);
  avformat_close_input(&pFormatCtx);  
  av_log(NULL, AV_LOG_INFO," %s,iRet=%d",__FUNCTION__,iRet);
  return iRet;
}
